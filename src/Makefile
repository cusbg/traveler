#
# File: Makefile
# Created: 2014-09-01 14:42
# By: eliasr <eliasr@u-pl22>
#


CC                      = g++
DEBUG                   = -g -Wall
#RELEASE                 = -DNDEBUG -O3
RELEASE = -O3
CFLAGS                  = -std=c++11 -c $(DEBUG) $(RELEASE)
LFLAGS                  = $(DEBUG) $(RELEASE)
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
run: build cleanLog cp
	sh -x run.sh


build: $(BUILDDIR)/$(TARGET)

debug: build
	sh -x run.sh debug

analyse:
	gprof $(BUILDDIR)/$(TARGET) | less


$(BUILDDIR)/$(TARGET): $(BUILDDIR)/Makefile

CORES = $(shell expr $(shell nproc) + 2)
$(BUILDDIR)/Makefile: $(SOURCES) $(HEADERS) $(addprefix $(BUILDDIR)/,$(MAKEFILES))
	@rm -f $@
	@echo -e \
			\\nbuild : $(TARGET) \\n \
			\\n \
			\\n$(TARGET) : $(OBJECTS) \\n\\t \
			@echo "****************LINKING:****************" \\n\\t \
			@echo \\n\\t \
			$(CC) $(LFLAGS) $(OBJECTS) -o $(TARGET) \\n\\t \
			@echo \\n\\t \
			@echo "**************END-LINKING:**************" \\n\\t \
			\\n \
			\\ninclude $(MAKEFILES) \
					>> $@
	@make -j$(CORES) --directory=$(BUILDDIR) -f Makefile

mk_dirs:
	@mkdir -p $(LOGDIR) $(FILESDIR) precomputed $(FILESDIR)/run $(FILESDIR)/run-op

cp: mk_dirs
	cp `find ../InFiles/ -maxdepth 1 -type f` precomputed/


$(BUILDDIR)/%.mk: %.cpp Makefile
	@mkdir -p $(BUILDDIR)
	@rm -f $@
	@$(MAKEDEPENDENCY) $(CFLAGS) $< | sed 's@\([^ :]*\.\([hc]pp\|hh\|h\)\)@../\1@g' \
					>> $@
	@echo '	$(CC) $(CFLAGS) $$< -o $$@ ' \
					>> $@

java_rted:
	java -cp java_RTED util.RTEDCommandLine --costs 1 1 0 --mapping --verbose --trees #'T1' 'T2'

rnafold:
	../RNAPlot/bin/RNAfold --noPS << END \n SEQ \n END


clean: cleanMK cleanCompiled cleanLog

cleanCompiled:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/$(TARGET)

cleanMK:
	rm -f $(BUILDDIR)/*.mk $(BUILDDIR)/Makefile

cleanLog:
	rm -f $(LOGDIR)/*.log

.PHONY: $(BUILDDIR)/Makefile clean cleanMK


