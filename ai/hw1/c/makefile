
CFLAGS=-I$(PCGDIR)/include
PCGDIR=pcg-c
PCGLIB=$(PCGDIR)/src/libpcg_random.a
BIN=main

all: $(BIN)

clean:
	$(RM) main.o $(BIN)

main.o: main.c

$(PCGDIR)/src/libpcg_random.a:
	$(MAKE) -C $(PCGDIR)

$(BIN): main.o $(PCGDIR)/src/libpcg_random.a
	$(CC) -o $@ $^
