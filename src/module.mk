
CC                      = g++
DEBUG                   = -g -Wall
CFLAGS                  = -std=c++11 -c ${DEBUG} ${RELEASE} -I${ROOTDIR}/include/ -I${ROOTDIR}/include/tests/
LFLAGS                  = ${DEBUG} ${RELEASE} -std=c++11
SHELL                   = /bin/bash -o pipefail

MAKEDEPENDENCY          = ${CC} -MM -E ${CFLAGS}

SOURCES                 = $(shell find ./ -maxdepth 1 -name "*.cpp" | sed 's@\./@@g')
MAKEFILES               = $(addsuffix .mk,${SOURCES})
OBJECTS                 = $(shell echo $(SOURCES) | sed 's@\.cpp@.o@ g; s@[^ ]*/@@g')
SHELL                   = /bin/bash
BUILDDIR                = ${ROOTDIR}/build
RUNSH                   = sh -x ${ROOTDIR}/make.sh

MODULEVAR               = $(shell echo ${MODULE} | tr 'a-z' 'A-Z')

make: $(addprefix ${BUILDDIR}/,${MAKEFILES})
	echo $(addprefix ${BUILDDIR}/,${MAKEFILES})
	@echo -e \
		\\ninclude ${MAKEFILES} \
		\\n \
		\\n${MODULEVAR} = ${OBJECTS} \
		\\n \
			>> ${BUILDDIR}/Makefile


testmake: prepare_test make

prepare_test:
	$(eval CFLAGS := ${CFLAGS} -DTESTS)


${BUILDDIR}/%.cpp.mk: %.cpp FORCEREBUILD
	@rm -f $@
	@${MAKEDEPENDENCY} $< | \
		sed "s@\([^ :]*\.cpp\)@${ROOTDIR}/${MODULE}/\1@g" \
			>> $@ || rm -rf $@
	@echo -e \
		\\t ${CC} ${CFLAGS} $$\< -o $$\@ \
			>> $@ || rm -rf $@

build:
	make --directory=${ROOTDIR} --file=Makefile $@

run:
	make --directory=${ROOTDIR} --file=Makefile $@

debug:
	make --directory=${ROOTDIR} --file=Makefile $@

test:
	make --directory=${ROOTDIR} --file=Makefile $@

clean:
	make --directory=${ROOTDIR} --file=Makefile $@

FORCEREBUILD:

.PHONY: make testmake *.mk FORCEREBUILD
