src := p.tex

all:
	xelatex ${src} # generate biber files
	biber $(basename ${src}) # generate citations
	xelatex ${src} # format citations
	xelatex ${src} # page breaks

clean:
	${RM} *.bbl *.blg *.bcf *.aux *.log *.out *.run.xml
