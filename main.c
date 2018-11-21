// Begin includes
#include <iostream>
#include <string>
#include "string.h"
#include "lex.yy.h"
#include "AST.h"
#include "shell.tab.h"
// End includes

void print_intro() {
  std::cout
  << "**************************************" << "\n"
  << "*        Welcome to KOSH shell       *" << "\n"
  << "* Enter input commands and arguments *" << "\n"
  << "* Type \"exit\" to exit the shell      *" << "\n"
  << "**************************************" << std::endl;
}

int main() {
  print_intro();
  for (;;) {
    std::string in_str;
    std::cout << ">> ";
    std::getline(std::cin, in_str);
    const int len = in_str.length()+1;
    char in_str_copy[len];
    strcpy(in_str_copy, in_str.c_str());
    std::cout << "Input string is:\n'" << in_str << "'" <<std::endl;
    YY_BUFFER_STATE bp = yy_scan_string(in_str_copy);
    if (in_str == "exit") {yy_delete_buffer(bp); break;}
    yyparse();
    yy_delete_buffer(bp);

  }
}
