CXX = g++
CXXFLAGS = -std=c++14 -Wall -g -pedantic	-Wextra

shell: main.o lex.yy.o shell.tab.o
	$(CXX) $(CXXFLAGS) -o $@ $^
main.o: main.c lex.yy.h shell.tab.h
	$(CXX) $(CXXFLAGS) -c -o $@ main.c
lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
shell.tab.o: shell.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
shell.tab.c shell.tab.h: shell.y
	bison -d -v $^
lex.yy.c lex.yy.h: shell.l
	flex $^





.PHONY: clean
clean:
		rm -f shell *.o lex.yy.* shell.output shell.tab.*
