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

$(BUILDDIR)/Makefile: $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES)) Makefile
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
	@$(MAKEDEPENDENCY) $^ | sed 's@\([^. :]*.\([hc]pp\|hh\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@


clean:
	rm -rf $(BUILDDIR)

cleanmk:
	rm -f $(BUILDDIR)/*.mk Makefile

.PHONY: $(BUILDDIR)/Makefile clean


