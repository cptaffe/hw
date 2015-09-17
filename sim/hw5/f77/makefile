
FSRC=main.f
CFSRC=${FSRC:.f=.c}
CSRC=$(CFSRC) rand.c
LIBS=-lf2c
BIN=main

all:
	f2c -c $(FSRC)
	$(CC) $(CSRC) $(LIBS) -o $(BIN)
