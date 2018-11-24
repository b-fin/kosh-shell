// Interface for the Shell class. This will encapsulate the functionality
//  of the shell and make it easier to manage things like symbol tables
//  (for variables), I/O (for redirects), etc.

// Begin header guards:
#ifndef _SHELL_H
#define _SHELL_H


// Begin includes:
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "AST.h"
#include "lex.yy.h"
#include "shell.tab.h"
// End includes

class Shell {
private:
    // Program_node *m_program_node; // Potentially
    // Symbol_table *m_symbol_table;
    // Environment_table *m_environ_table;
    // Process_handler *m_process_handler;
    // IO_handler *m_io_handler;

    // std::string m_input_string; // maybe for sending to system()?

public:
  Shell() = default;
  ~Shell() = default;

  int run();
  void print_intro();
};


#endif // End header guards
