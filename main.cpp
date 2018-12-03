#include <iostream>
#include "shell.tab.h"
#include <cstring>
#include "lex.yy.h"
#include "Shell.h"

int main(){
  //yydebug = 1;
  Shell *sh = new Shell();
//  bool big_exit;
  int exit_nana;
  do {
  //  big_exit = sh->_exit(); // if exit is executed, this is true; otherwise false
    exit_nana = sh->run();
  } while(!sh->shell_exit);
  delete sh;
  std::cout<< "Exiting KOSH with code " << exit_nana << ". Goodbye!" <<std::endl;
  return 0;
}
