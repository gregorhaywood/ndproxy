# Changes

I've had to make changes to handle multihoming.

## Build/Install:

```
$ make clean && make  DEBUG_FLAGS=-DDEBUG_NDPROXY
$ make  DEBUG_FLAGS=-DDEBUG_NDPROXY install
```

## Changes

* Modified sysctl interface:
    * Added net.inet6.ndproxy parent node
    * Switched to ' ' deliminator
    * .ndproxy.downlink_mac_list
	* Set the MAC address to provide as the downstream. Must be same order 
	    as .uplink_iface_list
    * .ndproxy.uplink_iface_list
	* Can now contain a list of upstream interface names, allowing 
	    multihoming
    * .ndproxy.exception_addr_list
	* As before, IPv6 addresses not to proxy
    * .ndproxy.uplink_mac_list
	* IPv6 addresses to reply to NS from. These values are used for all 
	    interfaces, so collisions are possible.
    * .ndproxy.packet_count
	* Register the hook
* Style changes in line with style(9)
* Changed to standard functions for parsing where possible



