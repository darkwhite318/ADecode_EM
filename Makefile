
all: ADecode_EM2

ADecode_EM2: ADecode_EM2.o
	g++ -o ADecode_EM2 ADecode_EM2.o -g -O3

ADecode_EM2.o: ADecode_EM2.c
	g++ -c ADecode_EM2.c -g -O3

