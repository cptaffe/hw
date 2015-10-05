
CFLAGS=-I$(PCGDIR)/include -std=c89 -Wpedantic -Wall -Wextra -Wformat=2 -Os
LDFLAGS=-lm -lpthread
BIN=main

all: $(BIN)

clean:
	$(RM) $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
