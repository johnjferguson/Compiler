output: Main.o Compiler.o Scanner.o Token.o
	g++ Main.o Compiler.o Scanner.o Token.o -o compiler

Main.o: Main.cpp
	g++ -c Main.cpp

Compiler.o: Compiler.cpp Compiler.h
	g++ -c Compiler.cpp

Scanner.o: Scanner.cpp Scanner.h
	g++ -c Scanner.cpp6

Token.o: Token.cpp Token.h
	g++ -c Token.cpp

clean:
	rm *.o output

