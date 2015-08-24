#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC				= g++
DEBUG			= -g -Wall
#RELEASE		= DNDEBUG -O3
CFLAGS			= -std=c++11 -c $(DEBUG) $(RELEASE)
LFLAGS			= $(DEBUG) $(RELEASE) $(shell pkg-config --libs log4cpp)
SHELL			= /bin/bash

BUILDDIR		= build
LOGDIR			= $(BUILDDIR)/logs
FILESDIR		= $(BUILDDIR)/files

TARGET			= program
SOURCES			= $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
HEADERS			= $(shell find ./ -name "*.hpp" | sed 's@\./@@g')
OBJECTS			= $(shell echo $(SOURCES) | sed 's@\.cpp@.o@ g; s@[^ ]*/@@g')
MAKEFILES		= $(shell echo $(SOURCES) | sed 's@\.cpp@.mk@g; s@[^ ]*/@@g')

MAKEDEPENDENCY	= $(CC) -MM -E 


ARGS			=
run: build $(FILESDIR)/generated
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

generate: build $(FILESDIR)/generated


$(BUILDDIR)/$(TARGET): OtherMks $(BUILDDIR)/Makefile

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
	@#@make --directory=$(BUILDDIR) -f Makefile
	@make -j4 --directory=$(BUILDDIR) -f Makefile

MK_DIRECTORIES		= $(LOGDIR) $(FILESDIR) $(FILESDIR)/mappings $(FILESDIR)/rted
mk_dirs:
	@mkdir -p $(MK_DIRECTORIES)

$(FILESDIR)/generated:
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	$(BUILDDIR)/$(TARGET) "generate" || rm -r $(FILESDIR)
	@touch $@

$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $^ | sed 's@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@

OtherMks:
	@make -j4 --directory=./tree -f Makefile build
	@make -j4 --directory=./ted	 -f Makefile build

rnafold:
	make -f ./Makefile.RNAfold

rted:
	make -f ./Makefile.RTED

clean:
	rm -rf $(BUILDDIR)

cleanMK:
	rm -f $(BUILDDIR)/*.mk $(BUILDDIR)/Makefile

cleanLog:
	rm -f $(LOGDIR)

.PHONY: $(BUILDDIR)/Makefile clean cleanMK


