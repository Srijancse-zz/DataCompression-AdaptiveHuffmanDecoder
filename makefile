CC=g++

all: ahencode ahdecode

ahencode: ahencode.cpp ahencode.h filter.o adaptiveHuffman.o
	$(CC) -o ahencode ahencode.cpp filter.o adaptiveHuffman.o

ahdecode: ahdecode.cpp ahdecode.h filter.o adaptiveHuffman.o
	$(CC) -o ahdecode ahdecode.cpp filter.o adaptiveHuffman.o

filter.o: filter.cpp filter.h
	$(CC) -c filter.cpp

adaptiveHuffman.o: adaptiveHuffman.cpp adaptiveHuffman.h
	$(CC) -c adaptiveHuffman.cpp

clean:
	rm *.o
