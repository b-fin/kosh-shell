CXX = g++
CXXFLAGS = -std=c++14 -Wall -g -pedantic	-Wextra

shell: main.o lex.yy.o shell.tab.o
		$(CXX) $(CXXFLAGS) -o $@ $^
lex.yy.c lex.yy.h: shell.l
	flex $^
shell.tab.c shell.tab.h: shell.y
	bison $^
shell.tab.o: shell.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
main.o: main.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^
.PHONY: clean
clean:
		rm -f shell *.o
