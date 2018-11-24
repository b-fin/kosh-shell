// Begin includes
#include <iostream>
#include <string>
#include "string.h"
#include "lex.yy.h"
#include "AST.h"
#include "shell.tab.h"
#include "Shell.h"
// End includes


int main() {
  Shell *sh = new Shell();
  int exit_status = sh->run();
  delete sh;


  return exit_status;
}
