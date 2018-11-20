// Implentation of AST.h methods.

// Begin includes:
#include "AST.h"
#include <assert.h>
// End includes

void Argument_node::print(){
  std::cout<< "(Argument node):" << std::endl <<
    "\tm_argument: " << m_argument << std::endl;
}




void Command_node::add_argument(Argument_node* argnode) {
  m_argument_count++;
  m_arguments.push_back(argnode);
}

void Command_node::print(){
  std::cout<< "(Command_node):\t" << std::endl <<
    "\tcmd_word: " << m_cmd_word << std::endl <<
    "\tm_argument_count: " << m_argument_count << std::endl;
  for(int i = 0; i<m_argument_count; i++) {
    std::cout<< "\tm_arguments["<<i<<"]: ";
    m_arguments[i]->print();
  } std::cout << std::endl;
}

Command_node::~Command_node(){
  if (!m_arguments.empty()) {
    for (uint i=0; i<m_arguments.size(); i++) {
      delete m_arguments[i];
    }
  }
}

void Program_node::print(){
  std::cout<< "(Program_node):\t" << std::endl;
  if (m_command) {
    m_command->print();
  }
}

Program_node::~Program_node() {
  if (m_command) {
    delete m_command;
  }
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
