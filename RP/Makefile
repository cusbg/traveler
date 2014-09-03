#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC = g++
CFLAGS = -std=c++11 -c -Wall
LFLAGS = -std=c++11

BUILDDIR = build

TARGET = program
SOURCES = $(shell find ./ -name "*.cpp" | sed 's@\./@@g')
HEADERS = $(shell find ./ -name "*.hpp" | sed 's@\./@@g')
OBJECTS = $(SOURCES:%.cpp=%.o)
MAKEFILES = $(SOURCES:%.cpp=%.mk)

MAKEDEPENDENCY = $(CC) -MM -E 



all: $(BUILDDIR)/$(TARGET)
	@echo "Makefile needs your attention"


$(BUILDDIR)/$(TARGET): $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES))
	@echo "****************COMPILING:****************"
	@cd $(BUILDDIR) && ls *.mk | while read file; do echo; if ! make -f $$file; then exit 5; fi; done #2>/dev/null
	@echo
	@echo "**************END-COMPILING:**************"
	@echo
			@# vypisuje nejaku hlasku o predefinovani targetu *.o... 
	@echo "****************LINKING:****************"
	$(CC) $(LFLAGS) $(addprefix $(BUILDDIR)/,$(OBJECTS)) -o $@ 
	@echo
	@echo "**************END-LINKING:**************"


$(BUILDDIR)/%.mk: %.cpp
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $^ | sed 's@\([^. :]*.[hc]pp\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@


build: $(BUILDDIR)/$(TARGET)


clean:
	rm -rf $(BUILDDIR)


run: build
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	$(BUILDDIR)/$(TARGET)
