#!/bin/bash

for module in $@
do
    make --directory=${module} --makefile=Makefile make
done
