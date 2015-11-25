
DOC=prop.tex
OUT=$(DOC:.tex=.pdf)

%.pdf: %.tex
	xelatex $^ \
	&& biber $(basename $^) \
	&& makeglossaries $(basename $^) \
	&& xelatex $^ \
	&& xelatex $^

all: $(OUT)

$(DOC): sections.sty letterbib.sty glosscompat.sty bib.bib

$(OUT): $(DOC)

clean:
	$(RM) *.aux *.blg *-blx.bib *.log *.out *.run.xml $(basename $(DOC)).bib $(basename $(DOC)).bbl *.pdf *.gls *.ist *.acn *.xdy *.glo *.glg *.alg *.acr *.bcf
