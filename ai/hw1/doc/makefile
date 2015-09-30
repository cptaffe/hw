DOC=doc.tex
OUT=$(DOC:.tex=.pdf)


%.pdf: %.tex
	xelatex $^
	xelatex $^

all: $(OUT)

clean:
	$(RM) $(OUT) $(OUT:.pdf=.log) $(OUT:.pdf=.aux)
