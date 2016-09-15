TARGET		= obhajoba
BUILDDIR	= .
IMGDIR		= ../img

build: ${TARGET}.pdf clean

run: build
	atril $(TARGET).pdf&

# LaTeX je potreba spustit nekolikrat, aby spravne spocital odkazy
${TARGET}.pdf: ${TARGET}.tex 
	pdflatex $< --output-directory .


clean: clear

clear:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
