CXX = g++
CXXFLAGS = -std=c++14 -Wall -g -O0 -pedantic	-Wextra -Wall

shell: main.o lex.yy.o shell.tab.o AST.o
	$(CXX) $(CXXFLAGS) -o $@ $^
main.o: main.c lex.yy.h shell.tab.h
	$(CXX) $(CXXFLAGS) -c -o $@ main.c
lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
shell.tab.o: shell.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
AST.o: AST.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
shell.tab.c shell.tab.h: shell.y
	bison -d -v $^
lex.yy.c lex.yy.h: shell.l
	flex $^





.PHONY: clean
clean:
		rm -f shell *.o lex.yy.* shell.output shell.tab.*
