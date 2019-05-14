CC = gcc
CFLAGS = -Wall

#Linkeo con -lm ya que math.h lo requiere en man
all: main.o
	$(CC) -o netpbm main.o operaciones.o netpbm.o cola_gen.o errors.o -lm

main.o: main.c operaciones.o errors.o cola_gen.o
	$(CC) $(CFLAGS) -c -o main.o main.c

operaciones.o: operaciones.c operaciones.h netpbm.o cola_gen.o errors.o
	$(CC) $(CFLAGS) -c -o operaciones.o operaciones.c

netpbm.o: netpbm.c netpbm.h errors.o
	$(CC) $(CFLAGS) -c -o netpbm.o netpbm.c
	
cola_gen.o: cola_gen.c cola_gen.h
	$(CC) $(CFLAGS) -c -o cola_gen.o cola_gen.c

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c -o errors.o errors.c
	
clean:
	$(RM) *.o netpbm
