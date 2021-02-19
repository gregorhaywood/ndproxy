/*-
 * Copyright (c) 2015 Alexandre Fenyo <alex@fenyo.net> - http://www.fenyo.net
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

#ifndef __NDCONF_H
#define __NDCONF_H

/* define DEBUG_NDPROXY to send debugging informations to the console. */
/* #define DEBUG_NDPROXY */

/* Max size of a MAC address: XX:XX:XX:XX:XX:XX = 17 chars. */
#define MACMAXSIZE 17

/* Max size of an IPv6 address: XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:YYY.YYY.YYY.YYY = 45 chars. */
#define IP6MAXSIZE 45

/*
 * Lower bound of exception addresses count to reserve space for. 
 * if more than 32 exception addresses are needed, you may have to
 * adjust this parameter.
 */
#define CONF_NEXCEPTIONS_MAX 32
/* Exceptions list: IPv6 addresses separated by ';', whole string is null terminated. */
#define CONF_NEXCEPTIONS_SIZE (CONF_NEXCEPTIONS_MAX * (IP6MAXSIZE + 1) + 1)

/*
 * Lower bound of uplink IPv6 addresses count to reserve space for.
 * if more than 32 uplink IPv6 addresses are needed, you may have to
 * adjust this parameter.
 */
#define CONF_NUPLINK_MAX 32
/* Exceptions list: IPv6 addresses separated by ';', whole string is null terminated. */
#define CONF_NUPLINK_SIZE (CONF_NUPLINK_MAX * (IP6MAXSIZE + 1) + 1)

/*
 * Downlink MAC addrs. List must be same order as interfaces. Seperated by ';'.
 */
#define CONF_NMAC_MAX (CONF_NUPLINK_MAX * (MACMAXSIZE + 1) + 1)

/* Packets handled counter. */
extern int ndproxy_conf_count;

/* Uplink interface names. List is seperated by ';'. */
#define CONF_IFLIST_MAX (CONF_NUPLINK_MAX * (IFNAMSIZ + 1) + 1)
extern char ndproxy_conf_str_uplink_interfaces[CONF_IFLIST_MAX];

/* Uplink router IPv6 link-local or global address. */
extern struct in6_addr ndproxy_conf_uplink_ipv6_addresses[CONF_NUPLINK_MAX];
extern int ndproxy_conf_uplink_ipv6_naddresses;

/* IPv6 link-local or global exceptions address list. */
extern struct in6_addr ndproxy_conf_exception_ipv6_addresses[CONF_NEXCEPTIONS_MAX];
extern int ndproxy_conf_exception_ipv6_naddresses;

/* Downlink router MAC address. */
extern struct ether_addr ndproxy_conf_downlink_mac_addresses[CONF_NMAC_MAX];
extern bool ndproxy_conf_downlink_mac_address_isset;

/* Uplink router MAC address. */
extern struct ether_addr ndproxy_conf_uplink_mac_address;
extern bool ndproxy_conf_uplink_mac_address_isset;

#endif
