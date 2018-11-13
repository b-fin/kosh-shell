//#include "shell_.main.h" // uncomment if using shell_main.h (I don't see why we need to)
#include <iostream>
#include <string>
#include "string.h"
#include "lex.yy.h"
#include "shell.tab.h"


int main() {
  // Convoluted way of getting input fron stdin as a single string for
  //  lex and bison to parse.
  std::string in_str;
  std::getline(std::cin, in_str);
  int len = in_str.length();
  char in_str_copy[len+1];
  strcpy(in_str_copy, in_str.c_str());
  YY_BUFFER_STATE bp = yy_scan_string( in_str_copy );
  yy_switch_to_buffer(bp);
  yyparse();
  yy_delete_buffer(bp);
}
