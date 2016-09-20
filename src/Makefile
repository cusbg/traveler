#
# File: src/Makefile
# Created: 2014-09-01 14:42
# By: Richard Eliáš <richard.elias@matfyz.cz>
#

TARGET                  = traveler
LOGDIR                  = $(BUILDDIR)/logs
ROOTDIR                 = .
BUILDDIR                = ${ROOTDIR}/build

MODULES                 = tree ted utils draw app tests
MODULESVARS             = $$\{TREE} $$\{TED} $$\{UTILS} $$\{DRAW} $$\{APP} $$\{TESTS}
MODULETARGET            = make

CC                      = g++
DEBUG                   = -g -Wall
RELEASE                 = -O3
LFLAGS                  = ${DEBUG} ${RELEASE} -std=c++11 -lrt
SHELL                   = /bin/bash -o pipefail


build: mkdir make_modules ${BUILDDIR}/Makefile compile

run: build cleanLog mkdir
	sh -x run.sh

doc:
	doxygen

debug: build mkdir
	sh -x run.sh debug

CORES                   = $(shell nproc)
compile:
	@echo "****************COMPILE:****************"
	@echo
	make -j${CORES} --directory=${BUILDDIR} -f Makefile build
	@echo
	@echo "**************END-COMPILE:**************"

make_modules: prepare_make ${MODULES}

prepare_make:
	@rm -rf ${BUILDDIR}/Makefile

${MODULES}:
	@echo "****************MAKEFILE-GENERATING:$@****************"
	@echo
	make --directory=$@ --makefile=Makefile ${MODULETARGET}
	@echo
	@echo "****************END-MAKEFILE-GENERATING:$@****************"

test: prepare_test force_rebuild_main build
	${BUILDDIR}/${TARGET}

prepare_test:
	$(eval MODULETARGET := testmake)

force_rebuild_main:
	@rm -rf ${BUILDDIR}/main* ${BUILDDIR}/${TARGET}

mkdir:
	@mkdir -p ${BUILDDIR}/files ${LOGDIR}

${BUILDDIR}/Makefile:
	@echo -e \
			build: ${TARGET} \\n\
			\\n${TARGET}: ${MODULESVARS} \\n\\t\
				@echo "****************LINKING:****************" \\n\\t \
				@echo \\n\\t \
				${CC} ${LFLAGS} ${MODULESVARS} -o ${TARGET} \\n\\t \
				@echo \\n\\t \
				@echo "**************END-LINKING:**************" \\n \
			\\nclean: \
				\\n\\trm -rf \*.o ${TARGET} \
					>>$@

sort:
	cat ${LOGDIR}/overlaps.log | sed 's/^.*DEBUG\] //' | sort -n -k 3 > ${LOGDIR}/overlaps.sorted.log


#OTHER GLOBAL targets
clean: cleanMK cleanCompiled cleanLog

cleanCompiled:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/$(TARGET)

cleanMK:
	rm -f $(BUILDDIR)/*.mk $(BUILDDIR)/Makefile

cleanLog:
	rm -f $(LOGDIR)/*.log


.PHONY: ${BUILDDIR}/Makefile clean cleanMK ${MODULES}
