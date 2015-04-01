#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC = g++
#DEBUG = -g
RELEASE = -DNDEBUG -O3
CFLAGS = -std=c++11 -c -Wall $(DEBUG) $(RELEASE)
LFLAGS = -std=c++11 -Wall $(shell pkg-config --libs log4cpp)
SHELL = /bin/bash

BUILDDIR = build

TARGET = program
SOURCES = $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
HEADERS = $(shell find ./ -name "*.hpp" | sed 's@\./@@g')
OBJECTS = $(SOURCES:%.cpp=%.o)
MAKEFILES = $(SOURCES:%.cpp=%.mk)

MAKEDEPENDENCY = $(CC) -MM -E 


run: build
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	$(BUILDDIR)/$(TARGET)


build: $(BUILDDIR)/$(TARGET)

debug: build
	gdb $(BUILDDIR)/$(TARGET)


$(BUILDDIR)/$(TARGET): $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES)) $(BUILDDIR)/Makefile

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
	@make --directory=$(BUILDDIR) -f Makefile


$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $^ | sed 's@\([^ :]*\.\([hc]pp\|hh\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@


clean:
	rm -rf $(BUILDDIR)

cleanMK:
	rm -f $(BUILDDIR)/*.mk Makefile

.PHONY: $(BUILDDIR)/Makefile clean cleanMK


