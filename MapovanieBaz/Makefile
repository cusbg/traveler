#
# File: Makefile
# Created: 2014-07-16 14:24
# By: eliasr <eliasr@u-pl22>
#


CC = g++
CFLAGS = -std=c++11 -c -Wall
LFLAGS = -std=c++11

SOURCES = files_read.cpp\
		  main.cpp \
		  types.cpp \
		  rnafold.cpp

HEADERS = files_read.hpp\
		  types.hpp \
		  rnafold.hpp



OBJECTS = $(SOURCES:%.cpp=%.o)
MAKEFILES = $(SOURCES:%.cpp=%.mk)

MAKEDIR = makefiles
BUILDDIR = build
TARGET = program



# build <- target
build: $(BUILDDIR)/$(TARGET)

# target <- objects
$(BUILDDIR)/$(TARGET): $(addprefix $(BUILDDIR)/,$(OBJECTS))
	$(CC) $(LFLAGS) $(addprefix $(BUILDDIR)/,$(OBJECTS)) -o $@

# main.o <- main.cpp
$(BUILDDIR)/main.o: main.cpp
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $@

# objects <- files.cpp + files.hpp
$(BUILDDIR)/%.o: %.cpp %.hpp
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $@

run: build
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	$(BUILDDIR)/$(TARGET)

clean:
	rm -rf $(BUILDDIR)
