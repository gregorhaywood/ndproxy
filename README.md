# ndproxy

This is a fork of https://github.com/AlexandreFenyo/ndproxy.

Kernel module for FreeBSD that implements IPv6 Neighbor Discovery proxying 
over Ethernet-like access networks.

With this module, your uplink provider continues to see you as a flat network, 
but you can subnet your IPv6 prefix and route packets to your preferred 
gateway (a Cisco router for instance), that may split your network into 
several subnets. To achieve this goal, your FreeBSD router will redirect 
packets coming from your uplink provider router to your gateway. You simply 
need to connect one of your FreeBSD host interfaces to the layer 2 that 
interconnects your provider and your gateway, and disable MLD snooping on the 
interconnection switch.

## Differences from Upstream

This fork adds support for multiple upstream routers. This can be necessary 
when using NPT to provide site multihoming, or when using locator rewriting 
with [ILNP](https://tools.ietf.org/html/rfc6740).

In addition, this fork reorders checks in the pfil hook so that irrelevant 
(non-ND) packets can be identified and discarded more efficiently.
