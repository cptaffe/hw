
OBJ= rand.o main.o
BIN= main
FC= gfortran
FFLAGS= -Wall -Wsurprising -pedantic -std=legacy -fopenmp
LDFLAGS= -lgomp

%.o: %.f
	$(FC) $(FFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(FC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) main
