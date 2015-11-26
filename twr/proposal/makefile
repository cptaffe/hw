
DOC=prop.tex
OUT=$(DOC:.tex=.pdf)


# Compilation pipeline.
# The first run of xelatex produces information needed for
# the bibliography and glossary sections.
# biber is run to generate the bibliography.
# makeglossaries is run to generate the glossaries,
# then xelatex is run again to generate glossary metadata
# for glossary references from glossary definitions.
# xelatex is run two more times to generate references and what not.
%.pdf: %.tex
	xelatex $^ \
	&& biber $(basename $^) \
	&& makeglossaries $(basename $^) \
	&& xelatex $^ \
	&& makeglossaries $(basename $^) \
	&& xelatex $^ \
	&& xelatex $^

all: $(OUT)

$(DOC): sections.sty letterbib.sty glosscompat.sty bib.bib

$(OUT): $(DOC)

clean:
	$(RM) *.aux *.blg *-blx.bib *.log *.out *.run.xml $(basename $(DOC)).bib $(basename $(DOC)).bbl *.pdf *.gls *.ist *.acn *.xdy *.glo *.glg *.alg *.acr *.bcf
