#
# File: Makefile
# Created: 2015-05-27 15:39
# By: Richard Eliáš <richard@ba30.eu>
#


SHELL			= /bin/bash
CC				= g++
DEBUG			= -g -Wall
#RELEASE		= DNDEBUG -O3
CFLAGS			= -std=c++11 -c $(DEBUG) $(RELEASE)

BUILDDIR		= ../build

SOURCES			= $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
MAKEFILES		= $(shell echo $(SOURCES) | sed 's@\.cpp@.mk@g; s@[^ ]*/@@g')

MAKEDEPENDENCY	= $(CC) -MM -E 

run: build
	@make --directory=../ -f Makefile run

build: $(addprefix $(BUILDDIR)/,$(MAKEFILES))

debug: build
	@make --directory=../ -f Makefile debug

$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $^ | sed 's@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../ted/\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@

clean:
	rm -rf $(BUILDDIR)

cleanMK:
	rm -f $(BUILDDIR)/*.mk Makefile

.PHONY: $(BUILDDIR)/Makefile clean cleanMK

