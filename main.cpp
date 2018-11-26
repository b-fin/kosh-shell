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
  extern bool AST_exit;
  do {
    sh->run();
  } while(!AST_exit);
  std::cout<< "[MAIN()] exiting KOSH" << std::endl;
  delete sh;

  return 0;
}
