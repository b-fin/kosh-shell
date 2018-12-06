#include <iostream>
#include "shell.tab.h"
#include <cstring>
#include "lex.yy.h"
#include "Shell.h"

int main(){
  //yydebug = 1;
  Shell *sh = new Shell();
  int exit_status;
  do {
    exit_status = sh->run();
  } while(!sh->shell_exit);
  delete sh;
  std::cout<< "Exiting KOSH with code " << exit_status << ". Goodbye!" <<std::endl;
  return 0;
}
