.include <minix.own.mk>

SUBDIR=	man1 man1x man2 man3 man4 man5 man6 man7 man8 man9

WHATISDBDIR?=	/usr/man

makedb:
	makewhatis ${WHATISDBDIR}
#	${TOOL_MAKEWHATIS} ${DESTDIR}${WHATISDBDIR}
# .if ${MKUNPRIVED} != "no"
# 	echo ".${WHATISDBDIR}/whatis.db type=file mode=0444 uname=${BINOWN} gname=${BINGRP}" \
# 	    | ${METALOG.add}
# .endif
# .else
# makedb:
# .endif

.include <minix.subdir.mk>
.include <minix.sys.mk>
