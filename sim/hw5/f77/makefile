
OBJ= rand.o main.o
BIN= main
FC= gfortran
FFLAGS= -Wall -Wsurprising -pedantic -std=legacy
LDFLAGS= -lgfortran

%.o: %.f
	$(FC) $(FFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) -o $@ -c $<

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) main
