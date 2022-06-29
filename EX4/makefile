.PHONY: clean all

all: sender measure

sender: sender.o
	gcc -Wall -g -o sender sender.o

measure : measure.o
	gcc -Wall -g -o measure measure.o

sender.o: sender.c
	gcc -Wall -g -c sender.c 

measure.o: measure.c
	gcc -Wall -g -c measure.c

clean:
	rm -f *.o *.a *.so sender measure