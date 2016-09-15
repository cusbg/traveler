TARGET		= obhajoba
BUILDDIR	= .

build: ${TARGET}.pdf clear

run: build
	atril $(TARGET).pdf&

# LaTeX je potreba spustit nekolikrat, aby spravne spocital odkazy
${TARGET}.pdf: ${TARGET}.tex 
	pdflatex $< --output-directory ${BUILDDIR}


clean: clear
	rm -f ${TARGET}.pdf

clear:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
