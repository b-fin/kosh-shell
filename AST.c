// Implentation of AST.h methods.

// Begin includes:
#include "AST.h"
#include <assert.h>
// End includes

void Command_node::add_argument(char* arg_string) {
  assert(arg_string);
  m_argument_count++;
  m_arguments.push_back(arg_string);
}

void Command_node::print(){
  std::cout<< "(Command_node):\t" << std::endl <<
    "\tcmd_word: " << m_cmd_word << std::endl <<
    "\tm_argument_count: " << m_argument_count << std::endl;
  for(int i = 0; i<m_argument_count; i++) {
    std::cout<< "\tm_arguments["<<i<<"]: " <<m_arguments[i];
  } std::cout << std::endl;
}

void Program_node::print(){
  std::cout<< "(Program_node):\t" << std::endl;
  m_command->print();
}

// THE FOLLOWING MAIN FUNCTION TESTED THE USAGE OF THE AST CLASSES
/*
int main() {
// We want to test to make sure this shit works, so:
// allocate a new command node and attach it to a new program node.
// print both. deallocate.
  char* new_word = new char[7]{'a','p','p','l','e','s','\0'};
  Command_node *cmd = new Command_node(new_word);
  cmd->print();
  char* new_arg1 = new char[3]{'-','i','\0'};
  char* new_arg2 = new char[3]{'-','a','\0'};
  cmd->add_argument(new_arg1);
  cmd->print();
  cmd->add_argument(new_arg2);
  cmd->print();

  Program_node *prgm = new Program_node(*cmd);
  prgm->print();

  delete prgm;
  delete[] new_arg2;
  delete[] new_arg1;
  delete cmd;
  delete[] new_word;


}
*/
