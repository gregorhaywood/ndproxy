# $FreeBSD$

PORTNAME=	ndproxy
PORTVERSION=	4.0
CATEGORIES=	net

MAINTAINER=	gh66@st-andrews.ac.uk
COMMENT=	Implementation of an IPv6 Neighbor Discovery Protocol proxy

LICENSE=	BSD2CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		kmod

SUB_FILES=	pkg-message

USE_RC_SUBR=	ndproxy
USE_GITHUB=	yes
GH_ACCOUNT=	gregorhaywood
GH_TAGNAME=	v${PORTVERSION}

# build for a kernel with 'options VIMAGE'
CFLAGS+=	-DVIMAGE

PLIST_FILES=	${KMODDIR}/${PORTNAME}.ko man/man4/${PORTNAME}.4.gz

pre-build:
	(cd ${BUILD_WRKSRC}; ${MAKE} depend)

do-install:
	${INSTALL_KLD} ${WRKSRC}/${PORTNAME}.ko ${STAGEDIR}${KMODDIR}
	${INSTALL_MAN} ${WRKSRC}/${PORTNAME}.4 ${STAGEDIR}${PREFIX}/man/man4/

.include <bsd.port.mk>
