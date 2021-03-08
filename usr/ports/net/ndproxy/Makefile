# $FreeBSD$

PORTNAME=	ndproxy
DISTVERSION=	4.0
# PORTVERSION=	g20210225
CATEGORIES=	net

MASTER_SITES=	http://alfred/ndproxy/

MAINTAINER=	gh66@st-andrews.ac.uk
COMMENT=	Implementation of an IPv6 Neighbor Discovery Protocol proxy

LICENSE=	BSD2CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		kmod

SUB_FILES=	pkg-message

USE_RC_SUBR=	ndproxy
# USE_GITHUB=	yes
# GH_ACCOUNT=	gregorhaywood
# GH_TAGNAME=	39e7fc6

# build for a kernel with 'options VIMAGE'
CFLAGS+=	-DVIMAGE

PLIST_FILES=	${KMODDIR}/${PORTNAME}.ko man/man4/${PORTNAME}.4.gz

pre-build:
	(cd ${BUILD_WRKSRC}; ${MAKE} depend)

do-install:
	${INSTALL_KLD} ${WRKSRC}/${PORTNAME}.ko ${STAGEDIR}${KMODDIR}
	${INSTALL_MAN} ${WRKSRC}/${PORTNAME}.4 ${STAGEDIR}${PREFIX}/man/man4/

.include <bsd.port.mk>
