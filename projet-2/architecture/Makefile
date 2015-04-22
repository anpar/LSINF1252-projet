all: pdf clean-aux

pdf:
	pdflatex arch.tex
	bibtex arch
	pdflatex arch.tex
	pdflatex arch.tex

clean-aux:
	rm *.log
	rm *.aux
	rm *.out
	rm *.bbl
	rm *.blg
	rm *.dvi
