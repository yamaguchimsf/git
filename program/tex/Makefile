TEX = platex
BIB = jbibtex
PDF = dvipdfmx

dvi:
	$(TEX) thesis
	$(BIB) thesis
	$(TEX) thesis
	$(TEX) thesis
pdf:thesis.dvi
	$(PDF) $^

clean:
	rm -f tex/*.aux 
	rm -f tex/*.tex~
	rm -f *.aux *.toc *.log *.dvi *.bbl *.blg

.PHONY:clean

