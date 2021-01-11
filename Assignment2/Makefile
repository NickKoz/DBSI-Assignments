#The compiler options(-g3 is needed for debugging) :
CFLAGS = -g3 -static 

#The final executable files:
PROG = prog

#The objects needed in order to compile the program:
OBJECTS = Record.o SHashTable.o HashTable.o HT_main.o BF_64.a


all : $(PROG)

$(PROG) : $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $(PROG)


clean :
	rm $(PROG) *.o HT1 SHT1


run : $(PROG)
	./$(PROG)

