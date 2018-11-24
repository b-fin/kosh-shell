// Implementation for the Shell class.

// Begin includes:
#include "Shell.h"
// End includes:

// Build up and initialize structures we'll need later, eg
//  symbol table, environment variables, etc.
/*
Shell::Shell() {

}

// Delete and free all the memory allocated above and during the course
//  of execution.
Shell::~Shell(){

}
*/
int Shell::run() {
  Shell::print_intro();
  std::string in_string;
  do {
    std::cout<< ">> ";
    std::getline(std::cin, in_string);
    char *str_copy = new char[in_string.size()+1];
    strcpy(str_copy, in_string.c_str());
    std::cout << "Input string is:\n'" << in_string << "'" <<std::endl;
    YY_BUFFER_STATE bp = yy_scan_string(str_copy);
    if (in_string == "exit") {
      yy_delete_buffer(bp);
      delete[] str_copy;
      break; // Placeholder
    }
    yyparse();
    yy_delete_buffer(bp);
    delete[] str_copy;


  } while(true);
  return 0;
}




void Shell::print_intro() {
  std::cout
  << "**************************************" << "\n"
  << "*        Welcome to KOSH shell       *" << "\n"
  << "* Enter input commands and arguments *" << "\n"
  << "* Type \"exit\" to exit the shell      *" << "\n"
  << "**************************************" << std::endl;
}
