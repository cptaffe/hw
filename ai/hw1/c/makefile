
PCGDIR=pcg-c

all:
	$(MAKE) -C $(PCGDIR)
	$(CC) main.c $(PCGDIR)/src/libpcg_random.a -I$(PCGDIR)/include
