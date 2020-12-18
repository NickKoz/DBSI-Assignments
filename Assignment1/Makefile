#The compiler options(-g3 is needed for debugging) :
CFLAGS = -g3 -static 

#The final executable files:
HT_PROG = prog_ht
HP_PROG = prog_hp

#The objects needed in order to compile the program:
HT_OBJECTS = Record.o HashTable.o HT_main.o BF_64.a
HP_OBJECTS = Record.o Heap.o HP_main.o BF_64.a


all : $(HT_PROG) $(HP_PROG)

$(HT_PROG) : $(HT_OBJECTS)
	gcc $(CFLAGS) $(HT_OBJECTS) -o $(HT_PROG)

$(HP_PROG) : $(HP_OBJECTS)
	gcc $(CFLAGS) $(HP_OBJECTS) -o $(HP_PROG)

clean_hp :
	rm $(HP_PROG) *.o

clean_ht :
	rm $(HT_PROG) *.o

clean :
	rm $(HP_PROG) $(HT_PROG) *.o


run_ht : $(HT_PROG)
	./$(HT_PROG)

run_hp : $(HP_PROG)
	./$(HP_PROG)
