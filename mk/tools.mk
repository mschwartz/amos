TOPDIR=		$(shell git rev-parse --show-toplevel)

include		$(TOPDIR)/mk/build-tools.mk

INCLUDE_PATH=	-I$(TOPDIR)/kernel  -I$(TOPDIR)/kernel/include -I$(TOPDIR)/.
