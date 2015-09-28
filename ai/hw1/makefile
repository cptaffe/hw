
CFLAGS=-I$(PCGDIR)/include -std=c99 -Wpedantic -Wall -Wextra -Wformat=2
LDFLAGS=-lm -lpthread
PCGDIR=pcg-c
PCGLIB=$(PCGDIR)/src/libpcg_random.a
BIN=main

all: $(BIN)

clean:
	$(RM) $(BIN)

$(PCGDIR)/src/libpcg_random.a:
	$(MAKE) -C $(PCGDIR)

$(BIN): main.c $(PCGDIR)/src/libpcg_random.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
