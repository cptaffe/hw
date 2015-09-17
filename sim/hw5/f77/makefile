
OBJ= rand.o main.o
BIN= main

%.o: %.f
	gfortran -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	gfortran $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) main
