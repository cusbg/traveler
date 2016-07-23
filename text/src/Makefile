TARGET		= prace
BUILDDIR	= .
IMGDIR		= ../img

build: ${TARGET}.pdf

run: build
	atril $(TARGET).pdf&

# LaTeX je potreba spustit nekolikrat, aby spravne spocital odkazy
${TARGET}.pdf: ${TARGET}.tex *.tex *.bib */*.tex
	${SHELL} run.sh ${TARGET} ${BUILDDIR}


clean: clear
	rm -f ${TARGET}.pdf

clear:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
