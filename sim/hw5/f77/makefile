
OBJ= rand.a main.o
BIN= main
FORT= gfortran
FFLAGS= -Wall -Wsurprising -pedantic -std=legacy

%.o: %.f
	$(FORT) $(FFLAGS) -c -o $@ $<

%.o: %.go
	gccgo -o $@ -c $<

$(BIN): $(OBJ)
	$(FORT) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) main
