#include <iostream>
#include "shell.tab.h"
#include <cstring>
#include "lex.yy.h"

int main(){
  yydebug = 1;
  extern Program *root;
  std::string instring;
  std::getline(std::cin,instring);
  char *str_copy = new char[instring.size()+1];
  strcpy(str_copy, instring.c_str());
  /*std::cout << "Input string is:\n'" << in_string << "'" <<std::endl; */
  YY_BUFFER_STATE bp = yy_scan_string(str_copy);
  yyparse();
  root->print();
  yy_delete_buffer(bp);
  delete[] str_copy;
  delete root;
}
