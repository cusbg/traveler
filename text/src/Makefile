TARGET		= prace
BUILDDIR	= .
LATEX_CMD	= pdflatex ${TARGET}.tex --output-directory ${BUILDDIR}
BIBTEX_CMD	= bibtex ${TARGET}

build: ${TARGET}.pdf clear

run: build
	atril $(TARGET).pdf&

# LaTeX je potreba spustit nekolikrat, aby spravne spocital odkazy
${TARGET}.pdf: ${TARGET}.tex *.tex *.bib */*.tex
	${LATEX_CMD}
	${BIBTEX_CMD}
	${LATEX_CMD}
	${LATEX_CMD}

clean: clear
	rm -f ${TARGET}.pdf

clear:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
