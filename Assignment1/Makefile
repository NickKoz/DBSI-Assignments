#The compiler options(-g3 is needed for debugging) :
CFLAGS = -g3 -static 

#The final executable file:
PROGRAM = prog

#The objects needed in order to compile the program:
OBJECTS = Heap.o main.o BF_64.a


$(PROGRAM) : $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $(PROGRAM)

clean :
	rm $(PROGRAM) *.o
	rm HP1

run : $(PROGRAM)
	./$(PROGRAM)