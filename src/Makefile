#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC                      = g++
DEBUG                   = -g -Wall
#RELEASE                 = -DNDEBUG -O3
CFLAGS                  = -std=c++11 -c $(DEBUG) $(RELEASE)
LFLAGS                  = $(DEBUG) $(RELEASE) $(shell pkg-config --libs log4cpp)
SHELL                   = /bin/bash -o pipefail

BUILDDIR                = build
LOGDIR                  = $(BUILDDIR)/logs
FILESDIR                = $(BUILDDIR)/files

TARGET                  = program
SOURCES                 = $(shell find ./ -maxdepth 1 -name "*.cpp" | sed 's@\./@@g')
HEADERS                 = $(shell find ./ -name "*.hpp" | sed 's@\./@@g')
OBJECTS                 = $(shell echo $(SOURCES) | sed 's@\.cpp@.o@ g; s@[^ ]*/@@g')
MAKEFILES               = $(shell echo $(SOURCES) | sed 's@\.cpp@.mk@g; s@[^ ]*/@@g')

MAKEDEPENDENCY          = $(CC) -MM -E 


ARGS                    =
run: build
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	$(BUILDDIR)/$(TARGET) $(ARGS)


build: mk_dirs $(BUILDDIR)/$(TARGET)

debug: build
	gdb $(BUILDDIR)/$(TARGET)

analyse:
	gprof $(BUILDDIR)/$(TARGET) | less


$(BUILDDIR)/$(TARGET): $(BUILDDIR)/Makefile

$(BUILDDIR)/Makefile: $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES))
	@rm -f $@
	@echo -e\
			\\nbuild : $(TARGET) \\n \
			\\n \
			\\n$(TARGET) : $(OBJECTS) \\n\\t \
			@echo "****************LINKING:****************" \\n\\t \
			@echo \\n\\t \
			$(CC) $(LFLAGS) $(OBJECTS) -o $(TARGET) \\n\\t \
			@echo \\n\\t \
			@echo "**************END-LINKING:**************" \\n\\t \
			@echo \\n\\t \
			\\n \
			\\nCOMPBEGIN: \\n\\t \
			@echo \\n\\t \
			@echo "****************COMPILING:****************" \\n\\t \
			@echo \
			\\n \
			\\nCOMPEND: \\n\\t \
			@echo \\n\\t \
			@echo "**************END-COMPILING:**************" \\n\\t \
			@echo \
			\\n \
			\\ninclude $(MAKEFILES) \
					>> $@
	@make -j6 --directory=$(BUILDDIR) -f Makefile

MK_DIRECTORIES		= $(LOGDIR) $(FILESDIR)
mk_dirs:
	@mkdir -p $(MK_DIRECTORIES)
	@cp -n precomputed/*.ps build/files/


$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $^ | sed 's@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@

java_rted:
	java -cp java_RTED util.RTEDCommandLine --costs 1 1 0 --mapping --verbose --trees #'T1' 'T2'

rnafold:
	../RNAPlot/bin/RNAfold --noPS << END \n SEQ \n END


clean:
	rm -rf $(BUILDDIR)

cleanMK:
	rm -f $(BUILDDIR)/*.mk $(BUILDDIR)/Makefile

cleanLog:
	rm -f $(LOGDIR)

.PHONY: $(BUILDDIR)/Makefile clean cleanMK


