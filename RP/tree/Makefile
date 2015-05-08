#
# File: Makefile
# Created: 2015-05-04 14:04
# By: eliasr <eliasr@u-pl22>
#


CC				= g++
DEBUG			= -g
#RELEASE		= DNDEBUG -O3
CFLAGS			= -std=c++11 -c -Wall $(DEBUG) $(RELEASE)
SHELL			= /bin/bash

BUILDDIR		= ../build

SOURCES			= $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
MAKEFILES		= $(shell echo $(SOURCES) | sed 's@\.cpp@.mk@g; s@[^ ]*/@@g')

MAKEDEPENDENCY	= $(CC) -MM -E 

build: $(addprefix $(BUILDDIR)/,$(MAKEFILES))

run: build
	@make --directory=../ -f Makefile run

$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $^ | sed 's@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../tree/\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@

clean:
	rm -rf $(BUILDDIR)

cleanMK:
	rm -f $(BUILDDIR)/*.mk Makefile

cleanLog:
	rm -f $(LOGDIR)

.PHONY: $(BUILDDIR)/Makefile clean cleanMK

