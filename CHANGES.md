# Changes

I've had to make changes to handle multihoming.

## Build/Install:

```
$ make clean && make  DEBUG_FLAGS=-DDEBUG_NDPROXY
$ make  DEBUG_FLAGS=-DDEBUG_NDPROXY install
```

## Changes

* Support for multiple upstream ifaces

TODO
* Support for multiple upstream mac addresses
