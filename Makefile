CXX = g++
CXXFLAGS = -std=c++14 -Wall -g -O0 -pedantic -Wextra -Wall

shell: main.o lex.yy.o shell.tab.o AST.o
	$(CXX) $(CXXFLAGS) -o $@ $^
main.0: main.cpp lex.yy.h shell.tab.h
	$(CXX) $(CXXFLAGS) -c -o $@ main.cpp
lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
AST.o: AST.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^
shell.tab.o: shell.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
lex.yy.c lex.yy.h: shell.l shell.tab.h
	flex shell.l
shell.tab.c shell.tab.h: shell.y AST.h
	bison -d -v shell.y
.PHONY: clean
clean:
	rm -f shell *.0 lex.yy.* shell.output shell.tab.*
