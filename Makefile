all : prog clean

prog: main.o
	g++ ./421_chupakhin.o -o ./prog
	
main.o: ./421_chupakhin.cpp
	g++ -O2 -g -Wall -std=c++0x -c ./421_chupakhin.cpp
clean:
	rm ./421_chupakhin.o
