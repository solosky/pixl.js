CC=cc -std=c11
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I../..

all: k-nucleotide

k-nucleotide: k-nucleotide.exe data.txt
	./k-nucleotide.exe < data.txt

k-nucleotide.exe: nucleotide.c
	$(CC) $(CFLAGS) $(CPPFLAGS) nucleotide.c -o k-nucleotide.exe -lpthread

data.txt: fasta.exe
	./fasta.exe 25000000 > data.txt

fasta.exe: fasta.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -fopenmp fasta.c -o fasta.exe

clean:
	rm -f *.exe *~ *.o data.txt

