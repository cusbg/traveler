TARGET		= prace
BUILDDIR	= .

run: build
	@#

build: ${TARGET}.pdf


# LaTeX je potreba spustit nekolikrat, aby spravne spocital odkazy
${TARGET}.pdf: ${TARGET}.tex *.tex Makefile
	@mkdir -p ${BUILDDIR}
	${SHELL} run.sh ${TARGET} ${BUILDDIR}


clean:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
	rm -f ${TARGET}.pdf
