// Implentation of AST.h methods.

// Begin includes:
#include "AST.h"
#include <assert.h>
#include <stdlib.h>
#include <cstring>
// End includes

//=============================================================================
//=============================================================================
  ///////////////////////////
 // ARGUMENT NODE METHODS //
///////////////////////////
void Argument_node::print(){
  std::cout<< "(Argument node):" << std::endl <<
    "\tm_argument: " << m_argument << std::endl;
}

int Argument_node::get_length(){
  return m_argument.size();
}




std::string Argument_node::get_word(){
  return (m_argument + " ");
}
//=============================================================================
//=============================================================================

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
    }
  }
}

bool Command_node::has_arguments(){
  return (!m_arguments.empty());
}
/*
int Command_node::get_length(){
  return m_cmd_word.size();
}
*/
int Command_node::get_arg_count(){
  return m_argument_count;
}

std::string Command_node::arguments_string(){
  std::string result {};
  for (int i=0; i<m_argument_count; i++) {
    result += m_arguments[i]->get_word();
  }
  return result;
}

std::string Command_node::get_word() {
  return m_cmd_word;
}
/*
int Command_node::get_combined_arg_lengths(){
  int total_length = 0;
  for (int i = 0; i<m_argument_count; i++) {
    total_length += m_arguments[i]->get_length();
  }
  return total_length;
}
std::string Command_node::get_word(){
  return m_cmd_word;
}


char* Command_node::get_arguments(char* holder){
  char result[get_combined_arg_lengths()];
  for (int i = 0; i<m_argument_count; i++) {
    strcat(result, m_arguments[i]->get_word()); // ?????
  }
  strcpy(holder, result); // ??????
  return holder; // ?????
}*/
//=============================================================================
//=============================================================================

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
  }
}

std::string& Program_node::get_eval_string() {
  std::string result {};
  result = m_command->get_word();
  if (m_command->has_arguments()) {
    result += " " + m_command->arguments_string();
    }
  std::string& result_ref {result};
  return result_ref;
  }

int Program_node::eval(){
  std::string strhm = get_eval_string();
  std::cout << "(eval()) get_eval_string(): " <<
  "'" << strhm << "'" << std::endl;
  char *c_str = new char[get_eval_string().length()+1];
  std::strcpy(c_str, get_eval_string().c_str());
  int exit_status = system(c_str);
  std::cout << "Okay we called system(), it returned " << exit_status <<std::endl;
  return exit_status;

}
//=============================================================================
//=============================================================================
