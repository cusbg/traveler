#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC = g++
CFLAGS = -std=c++11 -c -Wall -g
LFLAGS = -std=c++11 -Wall -g $(shell pkg-config --libs log4cpp)

BUILDDIR = build

TARGET = program
SOURCES = $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
HEADERS = $(shell find ./ -name "*.hpp" | sed 's@\./@@g')
OBJECTS = $(SOURCES:%.cpp=%.o)
MAKEFILES = $(SOURCES:%.cpp=%.mk)

MAKEDEPENDENCY = $(CC) -MM -E -std=c++11


run: build
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	$(BUILDDIR)/$(TARGET)


build: $(BUILDDIR)/$(TARGET)


$(BUILDDIR)/$(TARGET): $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES)) $(BUILDDIR)/Makefile
	@echo "****************LINKING:****************"
	@echo
	$(CC) $(LFLAGS) $(addprefix $(BUILDDIR)/,$(OBJECTS)) -o $@ 
	@echo
	@echo "**************END-LINKING:**************"

$(BUILDDIR)/Makefile: $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES)) Makefile
	@echo "****************COMPILING:****************"
	@echo
	@rm -f $@
	@echo '$(TARGET) : $(OBJECTS)' >> $(BUILDDIR)/Makefile
	@echo 'include $(MAKEFILES)' >> $(BUILDDIR)/Makefile
	@make --directory=$(BUILDDIR) -f Makefile
	@echo
	@echo "**************END-COMPILING:**************"
	@echo


$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $^ | sed 's@\([^. :]*.\([hc]pp\|hh\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@


clean:
	rm -rf $(BUILDDIR)

cleanmk:
	rm -f $(BUILDDIR)/*.mk Makefile

.PHONY: $(BUILDDIR)/Makefile clean


