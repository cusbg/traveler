
CC                      = g++
DEBUG                   = -g -Wall
RELEASE                 = -O3
CFLAGS                  = -std=c++11 -c ${DEBUG} ${RELEASE} -I../include/
LFLAGS                  = ${DEBUG} ${RELEASE} -std=c++11
SHELL                   = /bin/bash -o pipefail

MAKEDEPENDENCY          = ${CC} -MM -E ${CFLAGS}

SOURCES                 = $(shell find ./ -maxdepth 1 -name "*.cpp" | sed 's@\./@@g')
MAKEFILES               = $(addsuffix .mk,${SOURCES})
OBJECTS                 = $(shell echo $(SOURCES) | sed 's@\.cpp@.o@ g; s@[^ ]*/@@g')
SHELL                   = /bin/bash
BUILDDIR                = ${ROOTDIR}/build
RUNSH                   = sh -x ${ROOTDIR}/make.sh
INCLUDEDIR              = -I${ROOTDIR}/include/

make: $(addprefix ${BUILDDIR}/,${MAKEFILES})
	@echo -e \
		\\n${MODULE}: ${OBJECTS} \
		\\n\\ttouch ${MODULE} \
		\\n \
		\\ninclude ${MAKEFILES} \
			>> ${BUILDDIR}/Makefile


${BUILDDIR}/%.cpp.mk: %.cpp
	@${MAKEDEPENDENCY} ${INCLUDEDIR} $< | \
		sed "s@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../${MODULE}/\1@g" \
			>> $@
	@echo -e \
		\\t ${CC} ${CFLAGS} ${INCLUDEDIR} $$\< -o $$\@ \
			>> $@


build:
	make --directory=${ROOTDIR} --file=Makefile $@

run:
	make --directory=${ROOTDIR} --file=Makefile $@

clean:
	make --directory=${ROOTDIR} --file=Makefile $@

.PHONY:make ${BUILDDIR}/%.cpp.mk
