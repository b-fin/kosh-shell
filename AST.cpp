// Implentation of AST.h methods.

// Begin includes:
#include "AST.h"
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include "unistd.h"
// End includes

bool AST_exit = 0;

  ///////////////////////////
 // ARGUMENT NODE METHODS //
///////////////////////////

void Argument_node::print(){
  std::cout<< "(Argument node):" << std::endl <<
    "\tm_argument: " << m_argument << std::endl;
}

std::string Argument_node::get_word(){
  return (m_argument);
}

  //////////////////////////
 // COMMAND NODE METHODS //
/////////////////////////

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
      std::cout << "deleted argument: " << i <<
        "in Command_node deconstructor" << std::endl;
    }
  }
}

bool Command_node::has_arguments(){
  return (!m_arguments.empty());
}

int Command_node::get_arg_count(){
  return m_argument_count;
}
// Retuns a single string concatenated from the node's arguments;
//  To be used by the Program_node::get_eval_string() method.
std::string Command_node::arguments_string(){
  std::string result {};
  for (int i=0; i<m_argument_count; i++) {
    result += m_arguments[i]->get_word() + " "; // Space to separate arguments
  }
  return result;
}

std::string Command_node::get_word() {
  return m_cmd_word;
}

std::string Command_node::get_argument_n(int n) {
  assert(n > 0 && n <= get_arg_count());
  return m_arguments[n]->get_word();
}


  //////////////////////////
 // PROGRAM NODE METHODS //
//////////////////////////

void Program_node::print(){
  std::cout<< "(Program_node):\t" << std::endl;
  if (m_command) {
    m_command->print();
  }
}

Program_node::~Program_node() {
  if (m_command) {
    delete m_command;
    std::cout<< "Deleted m_command (in Program_node deconstructor)" << std::endl;
  }
}

void Program_node::add_command_node(Command_node* cmdnode) {
  m_command = cmdnode;
}

int Program_node::eval() {
  std::string command_word = m_command->get_word();
  if (command_word == "cd") {
    return call_cd();
  } else if (command_word == "exit") {
    return call_exit();
  } else if (command_word == "source") {
    return call_source();
  } else if (command_word == "echo") {
    return call_echo();
  } else if (command_word == "pwd") {
    return call_pwd();
  } else {
    // Default call functionality (fork() and exec()?)
  } std::cout<< "Not a built-in!" << std::endl; // TODO
    return 0;
}

// Simplified cd command:
// cd [path];
// if no path argument, change to home directory.
// otherwise, send argument to chdir() and see what it does
int Program_node::call_cd() {
  assert(m_command->get_word() == "cd");
  std::cout<< "[CALL_CD()] we are in cd\t";
  if (!m_command->has_arguments()) {
    // Without arguments, cd changes pwd to HOME
    std::cout<< "[CALL_CD()] no arguments\t";
    int result = chdir("~");
    std::cout<< "[CALL_CD()] chdir(\"~\") returned: " << result << std::endl;
    return result;
  } else if (!(m_command->get_arg_count()==1)) {
    std::cout<< "[CALL_CD()] not exactly one argument\t";
    // If there isn't exactly one argument, return -1
    return -1;
  } else {
    // We know there's only one argument so we can get the arg string:
    std::cout<< "[CALL_CD()] exactly one argument\t";
    std::string arg_str = m_command->arguments_string();
    arg_str.pop_back(); // Remove trailing space from arguments_string()
    int result = chdir(arg_str.c_str());
    std::cout<< "[CALL_CD()] tried to change dir to "
      << "'" << arg_str << "'\t";
    return result;
  }
}

// Simplified exit command:
// exit [n]
// If no n argument, returns 0. Otherwise returns n.
int Program_node::call_exit() {
  assert(m_command->get_word() == "exit");
  std::cout<< "[CALL_EXIT()] we are in exit\t";
  if (!m_command->has_arguments()) {
    std::cout<< "[CALL_EXIT()] command has arguments \t";
    // Without arguments, return 0
    AST_exit = 1;
    return 0;
  } else {
    int temp = stoi(m_command->get_argument_n(1));
    std::cout<< "[CALL_EXIT()] has no arguments\t";
    AST_exit = 1;
    return temp;
  }
}

// Simplified echo command:
// echo [arguments]
// If no argument, print a newline. Otherwise, prints arguments contained
//  in the argument nodes.
int Program_node::call_echo(){
  assert(m_command->get_word() == "echo");
  std::cout<< "[CALL_ECHO()] we are in echo\t";
  if (!m_command->has_arguments()) {
    std::cout<< "[CALL_ECHO()] command has no arguments\t";
    std::cout<< std::endl;
    return 0;
  } else {
    std::cout<< m_command->arguments_string() << std::endl;
    return 0;
  }
}

// Simplified source command:
// source filename [arguments]
// If no filename, error.
// STUB: TO DO LATER
int Program_node::call_source(){
  assert(m_command->get_word() == "source");
  assert(m_command->has_arguments());
  std::cout<< "[CALL_SOURCE()] we are in source\t";
  return 0;
}

// Simplified pwd command:
// pwd
// No arguments
// Prints the pwd? Can't return it I imagine.
int Program_node::call_pwd(){
  assert(m_command->get_word() == "pwd");
  std::cout<< "[CALL_PWD()] we are in pwd\t";
  char *cwd_temp = new char[256];
  cwd_temp = getcwd(cwd_temp, 256);
  if (!cwd_temp) { return -1; } // getcwd() error
  std::cout<< cwd_temp << std::endl;
  delete[]cwd_temp;
  return 0;
}
