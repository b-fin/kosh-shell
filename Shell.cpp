// Implementation for the Shell class.

// Begin includes:
#include "Shell.h"
#include "shell.tab.h"
#include "lex.yy.h"
#include "unistd.h"
// End includes:

#define PATH_SIZE 256

int Shell::run() {
  std::string in_string;
  extern Program_node *root;

  char *line_prefix = new char[PATH_SIZE];
  line_prefix = getcwd( line_prefix, PATH_SIZE);
  if (!line_prefix) { return -1; } // cwd error
  do {
    // Handle empty commands ie lone carriage returns
    std::cout<< line_prefix << ": ";
    std::getline(std::cin, in_string);
  } while(in_string == "");
  char *str_copy = new char[in_string.size()+1];
  strcpy(str_copy, in_string.c_str());
  /*std::cout << "Input string is:\n'" << in_string << "'" <<std::endl; */
  YY_BUFFER_STATE bp = yy_scan_string(str_copy);
  yyparse();
  int exit_status = root->eval();
  yy_delete_buffer(bp);
  delete[] line_prefix;
  delete[] str_copy;
  delete root;


  /* std::cout<< "[RUN()] Deleted str_copy, deleted root, returning: " <<
    exit_status << std::endl; */
  return exit_status;
}




void Shell::print_intro() {
  std::cout
  << "**************************************" << "\n"
  << "*        Welcome to KOSH shell       *" << "\n"
  << "* Enter input commands and arguments *" << "\n"
  << "* Type \"exit\" to exit the shell      *" << "\n"
  << "**************************************" << std::endl;
}
