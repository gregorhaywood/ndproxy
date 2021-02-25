/*-
 * Copyright (c) 2015-2019 Alexandre Fenyo <alex@fenyo.net> - http://www.fenyo.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/module.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/pfil.h>
#include <net/ethernet.h>
#include <netinet/in.h>

#include <net/vnet.h>
#include <netinet6/ip6_var.h>

#include "ndconf.h"
#include "ndproxy.h"
#include "ndpacket.h"


char			sysctl_downlink_mac_list[DOWN_MAC_STR_MAX];
char			sysctl_iface_list[UP_IFACE_STR_MAX];
char			sysctl_exception_addr_list[EXCEPTION_STR_MAX];
char			sysctl_uplink_addr_list[EXCEPTION_STR_MAX];

static int		hook_added = false;
static pfil_hook_t	pfh_hook;

/*
 * Create and register the pfil hook.
 */
static void
register_hook()
{
	struct pfil_hook_args pha;
	struct pfil_link_args pla;
	
	if (hook_added)
		return;

	pha.pa_version = PFIL_VERSION;
	pha.pa_type = PFIL_TYPE_IP6;
	pha.pa_flags = PFIL_IN;
	pha.pa_modname = "ndproxy";
	pha.pa_ruleset = NULL;
	pha.pa_rulname = "default-in6";
	pha.pa_func = packet;
	pfh_hook = pfil_add_hook(&pha);

	pla.pa_version = PFIL_VERSION;
	pla.pa_flags = PFIL_IN | PFIL_HEADPTR | PFIL_HOOKPTR;
	pla.pa_hook = pfh_hook;
	pla.pa_head = V_inet6_pfil_head;

	if (pfil_link(&pla) == 0)
		hook_added = true;
}

/*
 * Remove the pfil hook (ignored if not registered).
 */
static void
unregister_hook()
{
	if (!hook_added)
		return;
	pfil_remove_hook(pfh_hook);
}

/*
 * Called when the module is loaded or unloaded.
 */
static int
event_handler(struct module *module, const int event, void *arg)
{
	switch (event) {
	case MOD_LOAD:
		register_hook();
#ifdef DEBUG_NDPROXY
		uprintf("NDPROXY loaded\n");
		printf("NDPROXY loaded\n");
#endif
		return 0;

	case MOD_UNLOAD:
		unregister_hook();
#ifdef DEBUG_NDPROXY
		uprintf("NDPROXY unloaded\n");
		printf("NDPROXY unloaded\n");
#endif
		return 0;

	default:
		return EOPNOTSUPP;
	}
}

/* declare module data */
static moduledata_t ndproxy_conf = {
    "ndproxy",     	/* Module name. */
    event_handler,	/* Event handler. */
    NULL		/* No extra data. */
};

/* declare sysctl interface used to configure the behaviour of the module */
DECLARE_MODULE(ndproxy, ndproxy_conf, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
SYSCTL_DECL(_net_inet6);

/*
 * Get or update the value of the sysctl node named
 * net.inet6.ndproxy.{uplink,exception}_addr_list
 */
static int
addr_list(SYSCTL_HANDLER_ARGS, int str_size, int nentries_max,
    struct in6_addr *out_addrs, int *out_count)
{
	struct in6_addr addrs[nentries_max];
	char buf[str_size];
	char *delim, *next;
	int change = true;
	int err, count = 0;

	if (req->newptr != NULL)
		strncpy(buf, (const char*)req->newptr, UP_IFACE_STR_MAX);
	else
		change = false;
	next = buf;
	while (change && *next != '\0') {
		delim = strchr(next, DELIM);
		if (delim != NULL)
			*delim = '\0';

		err = inet_pton(AF_INET6, next, &addrs[count]);
		if (err == 0)
			err = EINVAL;
		if (err != 1)
			return (err);
#ifdef DEBUG_NDPROXY
		printf("NDPROXY INFO: parsed: [ %s ]\n", next);
#endif
		count++;

		/* Break at end of list, or max entries. */
		if (delim == NULL || count >= nentries_max)
			break;
		else
			next = delim + 1;
	}

	if (count >= nentries_max)
		return EINVAL;
	
	if ((err = sysctl_handle_string(oidp, arg1, arg2, req)) != 0)
		return (err);

	/* Apply changes. */
	if (change) {
		bcopy(addrs, out_addrs, count * sizeof(struct in6_addr));
		*out_count = count;
	}
	return (err);
}

static int
exception_addr_list(SYSCTL_HANDLER_ARGS)
{
	return addr_list(oidp, arg1, arg2, req, EXCEPTION_STR_MAX, EXCEPTION_MAX,
	    exception_addrs, &exception_addrs_set);
}

static int
uplink_addr_list(SYSCTL_HANDLER_ARGS)
{
	return addr_list(oidp, arg1, arg2, req, UPLINK_STR_MAX, UPLINK_MAX,
	    uplink_addrs, &uplink_addrs_set);
}

static int
downlink_mac_list(SYSCTL_HANDLER_ARGS)
{
	struct ether_addr addrs[UP_IFACE_MAX];
	char buf[DOWN_MAC_STR_MAX];
	char *delim, *next;
	int err, i, count = 0;
	int change = true;
	unsigned int o0, o1, o2, o3, o4, o5;

	if (req->newptr != NULL)
		strncpy(buf, (const char*)req->newptr, UP_IFACE_STR_MAX);
	else
		change = false;
	next = buf;
	while (change && *next != '\0') {
		delim = strchr(next, DELIM);
		if (delim != NULL)
			*delim = '\0';

		/* Parse MAC address. */
		i = sscanf(next, "%x:%x:%x:%x:%x:%x",
		    &o0, &o1, &o2, &o3, &o4, &o5);
		if (i != 6)
			return (EINVAL);
		addrs[count].octet[0] = o0;
		addrs[count].octet[1] = o1;
		addrs[count].octet[2] = o2;
		addrs[count].octet[3] = o3;
		addrs[count].octet[4] = o4;
		addrs[count].octet[5] = o5;
#ifdef DEBUG_NDPROXY
		printf("NDPROXY INFO: parsed: [ %s ]\n", next);
#endif
		count++;
		if (delim == NULL || count >= UP_IFACE_MAX)
			break;
		else
			next = delim + 1;
	}

	/* Reached max entries. */
	if (count >= UP_IFACE_MAX)
		return EINVAL;

	if ((err = sysctl_handle_string(oidp, arg1, arg2, req)) != 0)
		return (err);

	/* Apply changes. */
	if (change) {
		bcopy(addrs, downlink_mac_addrs, count * sizeof(struct ether_addr));
		downlink_mac_addrs_set = count;
	}
	return (err);
}

static int
uplink_iface_list(SYSCTL_HANDLER_ARGS)
{
	char addrs[UP_IFACE_MAX][IFNAMSIZ];
	char buf[UP_IFACE_STR_MAX];
	char *delim, *next;
	int err, i, count = 0;
	int change = true;

	if (req->newptr != NULL)
		strncpy(buf, (const char*)req->newptr, UP_IFACE_STR_MAX);
	else
		change = false;
	next = buf;
	while (change && *next != '\0') {
		delim = strchr(next, DELIM);
		if (delim != NULL)
			*delim = '\0';

		strncpy(addrs[count], next, IFNAMSIZ);
#ifdef DEBUG_NDPROXY
		printf("NDPROXY INFO: parsed: [ %s ]\n", next);
#endif
		count++;
		if (delim == NULL || count >= UP_IFACE_MAX)
			break;
		else
			next = delim + 1;
	}

	/* Reached max entries. */
	if (count >= UP_IFACE_MAX)
		return EINVAL;

	if ((err = sysctl_handle_string(oidp, arg1, arg2, req)) != 0)
		return (err);

	/* Apply changes. */
	if (change) {
		for (i = 0; i < UP_IFACE_MAX; i++) {
			if (i < count)
				strncpy(up_ifaces[i], addrs[i], IFNAMSIZ);
			else if (up_ifaces[i][0] == '\0')
				break;
			else
				up_ifaces[i][0] = '\0';
		}
	}
	return (err);
}

static int
cb_count(SYSCTL_HANDLER_ARGS)
{
	register_hook();
#ifdef DEBUG_NDPROXY
	printf("NDPROXY INFO: count\n");
#endif
	return sysctl_handle_int(oidp, arg1, arg2, req);
}

/*
 * Create a new sysctl node and attach config entries.
 */
SYSCTL_NODE(_net_inet6, OID_AUTO, ndproxy, CTLFLAG_RW, 0, "NDPROXY Config Ctr");

SYSCTL_PROC(_net_inet6_ndproxy, OID_AUTO, downlink_mac_list,
	    CTLTYPE_STRING | CTLFLAG_RW, sysctl_downlink_mac_list, sizeof(sysctl_downlink_mac_list),
    downlink_mac_list, "S", "Downlink MAC Addresses");

SYSCTL_PROC(_net_inet6_ndproxy, OID_AUTO, uplink_iface_list,
    CTLTYPE_STRING | CTLFLAG_RW, sysctl_iface_list, sizeof(sysctl_iface_list),
    uplink_iface_list, "S", "Interfaces with uplinks");

SYSCTL_PROC(_net_inet6_ndproxy, OID_AUTO, exception_addr_list,
    CTLTYPE_STRING | CTLFLAG_RW, sysctl_exception_addr_list, sizeof(sysctl_exception_addr_list),
    exception_addr_list, "S", "IPv6 addresses NOT to proxy");

SYSCTL_PROC(_net_inet6_ndproxy, OID_AUTO, uplink_addr_list,
    CTLTYPE_STRING | CTLFLAG_RW, sysctl_uplink_addr_list, sizeof(sysctl_uplink_addr_list),
    uplink_addr_list, "S", "Uplink router addresses");

SYSCTL_PROC(_net_inet6_ndproxy, OID_AUTO, packet_count,
    CTLTYPE_INT | CTLFLAG_RW, &ndproxy_conf_count, 0, cb_count, "I",
    "fire an event");
