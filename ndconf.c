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
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>

#include "ndconf.h"

/* Uplink interface names. */
char up_ifaces[UP_IFACE_MAX][IFNAMSIZ];

/* Uplink router addrs. */
struct in6_addr uplink_addrs[UPLINK_MAX];
int uplink_addrs_set = 0;

/* Addrs to exclude from proxying. */
struct in6_addr exception_addrs[EXCEPTION_MAX];
int exception_addrs_set = 0;

/*
 * MAC addresses to supply as the downlink. Provide one MAC
 * addr per interface to handle multihoming.
 */
struct ether_addr downlink_mac_addrs[UP_IFACE_MAX];
int downlink_mac_addrs_set = 0;

/* MAC addresses of uplink routers. */
struct ether_addr uplink_mac_addrs[UPLINK_MAX];
int up_mac_addrs_set = 0;

/* packets handled counter. */
int ndproxy_conf_count = 0;
