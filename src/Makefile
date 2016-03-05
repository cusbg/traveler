#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#

CC                      = g++
DEBUG                   = -g -Wall
RELEASE                 = -O3
CFLAGS                  = -std=c++11 -c ${DEBUG} ${RELEASE} -I../include/
LFLAGS                  = ${DEBUG} ${RELEASE} -std=c++11
SHELL                   = /bin/bash -o pipefail

MODULES                 = tree ted utils draw main

LOGDIR                  = $(BUILDDIR)/logs
ROOTDIR                 = .
BUILDDIR                = ${ROOTDIR}/build
TARGET                  = program

#GLOBAL targets
run: build cleanLog
	sh -x run.sh

build: mk_dirs ${BUILDDIR}/Makefile compile

make_modules:
	sh -x make.sh make ${MODULES}

${BUILDDIR}/Makefile:
	@rm -f $@
	@echo -e \
			build: ${TARGET} \\n\
			\\n${TARGET}: ${MODULES} \\n\\t\
				@echo "****************LINKING:****************" \\n\\t \
				@echo \\n\\t \
				${CC} ${LFLAGS} \*.o -o ${TARGET} \\n\\t \
				@echo \\n\\t \
				@echo "**************END-LINKING:**************" \\n \
			\\nclean: \
				\\n\\trm -rf \*.o ${TARGET} \
					>> $@

CORES                   = 2
compile: make_modules
	@echo "****************COMPILE:****************"
	@echo
	make -j${CORES} --directory=${BUILDDIR} -f Makefile
	@echo
	@echo "**************END-COMPILE:**************"


#OTHER GLOBAL targets
mk_dirs:
	@mkdir -p $(LOGDIR)

clean: cleanMK cleanCompiled cleanLog

cleanCompiled:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/$(TARGET)

cleanMK:
	rm -f $(BUILDDIR)/*.mk $(BUILDDIR)/Makefile

cleanLog:
	rm -f $(LOGDIR)/*.log

.PHONY: ${BUILDDIR}/Makefile ${BUILDDIR}/*.mk clean cleanMK
