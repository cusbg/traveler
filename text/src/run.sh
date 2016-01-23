#!/bin/sh

TARGET="$1"
OUTPUTDIR="$2"
TARGET_TEX="${TARGET}.tex"

fail_funct() {
	echo "run failed"
	exit 1
}

run_pdflatex() {
	pdflatex -output-directory ${OUTPUTDIR} ${TARGET_TEX} \
		|| fail_funct
}
run_bibtex() {
	bibtex ${TARGET} \
		|| fail_funct
}

run() {
	run_pdflatex
	run_bibtex
	run_pdflatex
	run_pdflatex
	make clear
}

run

