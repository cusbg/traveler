#
# File: Makefile
# Created: 2015-03-31 21:58
# By: Richard Eliáš <richard@ba30.eu>
#

TARGET		= util/RTEDCommandLine
INFILE1		= ../../InFiles/human.ps
INFILE2		= ../../InFiles/mouse.ps
ARGUMENTS	= --costs 1 1 0 -f $(INFILE1) $(INFILE2) -m -v

build : run

run:
	@echo "**********************************************************************"
	@echo "********************* RUNNING  PROGRAM *******************************"
	@echo "**********************************************************************"
	@echo
	java $(TARGET) $(ARGUMENTS)
