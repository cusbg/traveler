
CC                      = g++
DEBUG                   = -g -Wall
CFLAGS                  = -std=c++11 -c $(DEBUG) $(RELEASE)
LFLAGS                  = $(DEBUG) $(RELEASE)
SHELL                   = /bin/bash -o pipefail

CMD=make --directory=.. -f Makefile
TARGET=../build/program

build:
	$(CMD) TESTS

run: build
	$(CMD) run
	rm $(TARGET)

