.PHONY: clean all

all: myping sniffer

myping: myping.o
	g++ -Wall -g -o myping myping.o

sniffer: sniffer.o
	g++ -Wall -g -o sniffer sniffer.o

sniffer.o: sniffer.cpp
	g++ -Wall -g -c sniffer.cpp

myping.o: myping.cpp
	g++ -Wall -g -c myping.cpp

clean:
	rm -f *.o *.a *.so myping sniffer