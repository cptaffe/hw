
OBJ= rand.o main.o
BIN= main
FORT= gfortran
FFLAGS= -Wall -Wsurprising -pedantic -std=legacy

%.o: %.f
	$(FORT) $(FFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(FORT) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) main
