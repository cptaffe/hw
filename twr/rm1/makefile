DOC=doc.tex doc_d1.tex
OUT=$(DOC:.tex=.pdf)


%.pdf: %.tex
	xelatex $^
	xelatex $^

all: $(OUT)

clean:
	$(RM) $(OUT) $(OUT:.pdf=.log) $(OUT:.pdf=.aux)
