CATEGORY = graphics
NAME     = MesaLib
VERSION  = 6.2.1

SOURCE1 = ${NAME}-${VERSION}.tar.gz
SRCS = ${SOURCE1}
MASTERSITE = http://www.monaos.org/downloads
#WORKDIR = ${NAME}-${VERSION}
WORKDIR = Mesa-${VERSION}

MONA_TMPDIR = tmp
DSTDIR = .

PROCESSES = .FETCH .EXTRACT .PATCH .MAKE .INSTALL
all    : .MAKE
fetch  : .FETCH
extract: .EXTRACT
patch  : .PATCH
make   : .MAKE
install: .INSTALL
clean  :
	rm -rf tmp ${WORKDIR} ${PROCESSES}
distclean: clean
	@for f in ${SRCS}; do rm -f ${DSTDIR}/$${f}; done

.FETCH: ${SRCS}
	@if [ ! -f $@ ]; then touch $@; fi
DOWNLOAD = mkdir -p ${MONA_TMPDIR} ${DSTDIR} && cd ${MONA_TMPDIR} && rm -f $@ && wget
${SOURCE1}:
	@if [ ! -f ${DSTDIR}/$@ ]; then \
	  (${DOWNLOAD} ${MASTERSITE}/$@) && \
	  mv ${MONA_TMPDIR}/$@ ${DSTDIR}; \
	fi

.EXTRACT: .FETCH
	tar xvzf ${DSTDIR}/${SOURCE1}
	touch $@

.PATCH: .EXTRACT patch-aa
	cp bin/mklib ${WORKDIR}/bin
	cp configs/current ${WORKDIR}/configs
	cp configs/mona ${WORKDIR}/configs
	cp configs/mona-static ${WORKDIR}/configs
	patch -p0 < patch-aa
	touch $@

.MAKE: .PATCH
	cd ${WORKDIR} && make mona
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 ${WORKDIR}/lib/*.DLL ${MONADIR}/bin/LIBS
	install -p ${WORKDIR}/lib/lib*.a ${MONADIR}/lib
	mkdir -p ${MONADIR}/include/GL
	install -p ${WORKDIR}/include/GL/*.h ${MONADIR}/include/GL
