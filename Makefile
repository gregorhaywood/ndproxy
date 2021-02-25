
# after having changed "#include" statements, call 'rm .depends; make depend' to create a new dependencies file

# compile without debug informations on console and without debugging symbols:
#   make clean && make
# compile with debug informations on console:
#   make clean && make DEBUG_FLAGS=-DDEBUG_NDPROXY

# use load & unload predefined targets to load & unload the module:
#   make load
#   make unload

# use install target to copy the module to /boot/kernel and generate hints for the kernel loader:
#   make install
# or, if compiled with debugging symbols:
#  make DEBUG_FLAGS=-DDEBUG_NDPROXY install

# use maninstall target to install man page
# use manlint target to check manpage

# declare name of kernel module
KMOD    =  ndproxy

# build for a kernel with 'options VIMAGE'
CFLAGS += -DVIMAGE

# enumerate source files for kernel module
SRCS    = ndproxy.c ndpacket.c ndconf.c
MAN    += ndproxy.4

CLEANFILES += ndproxy.ko.debug ndproxy.ko.full

# Include kernel module makefile
.include <bsd.kmod.mk>
.include <bsd.man.mk>

man: all-man maninstall
	man ndproxy

pdf: ndproxy.4
	groff -man ndproxy.4 > ndproxy.ps
	ps2pdf ndproxy.ps

mandoc: ndproxy.4
	mandoc -Ttree ndproxy.4

distinfo:
	cd usr/ports/net/ndproxy && make makesum

