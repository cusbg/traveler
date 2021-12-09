#
# Richard Eliáš <richard.elias@matfyz.cz>
#


include ${ROOTDIR}/def.mk

MAKEDEPENDENCY          = ${CC} -MM -E ${CFLAGS}

SOURCES                 = $(shell find ./ -maxdepth 1 -name "*.cpp" | sed 's@\./@@g')
MAKEFILES               = $(addsuffix .mk,${SOURCES})
OBJECTS                 = $(shell echo $(SOURCES) | sed 's@\.cpp@.o@ g; s@[^ ]*/@@g')
BUILDDIR                = ${ROOTDIR}/build

MODULEVAR               = $(shell echo ${MODULE} | tr 'a-z' 'A-Z')

make: $(addprefix ${BUILDDIR}/,${MAKEFILES})
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


FORCEREBUILD:

.PHONY: make testmake *.mk FORCEREBUILD
