// Implementation file for AST structure

#include "AST.h"

//////////////////////////////////
// Arguments methods /////////////
//////////////////////////////////

void Arguments::add_argument(std::string in_str) {
  m_arguments.push_back(in_str);
  m_arg_count++;
}

void Arguments::print() const {
  std::cout<< "\t\t\t\t\t\t**** ARGUMENTS ****\n";
  for (int i=0; i<m_arg_count; i++) {
    std::cout<< "\t#" << i << ": " << m_arguments[i] << std::endl;
  }
  std::cout<< "\t\t\t\t\t\t**** END ARGUMENTS ****\n";
}

int Arguments::get_arg_count() const {
  return m_arg_count;
}
//////////////////////////////////
// Set methods ///////////////////
//////////////////////////////////

void Set::print() const {
  std::cout<< "**** SET ****\n"<<
  "\tVarname: " << m_varname << std::endl <<
  "\tValue: " << m_value << std::endl;
  std::cout<< "**** END SET ****\n";
}

//////////////////////////////////
// S_command methods /////////////
//////////////////////////////////

S_command::~S_command() {
  if (m_set) { delete m_set; }
  if (m_arguments) { delete m_arguments; }
}
/*
void S_command::add_arguments(const Arguments* in_args) {

} */

void S_command::print() const {
  std::cout<< "\n\t\t\t\t\t**** S_COMMAND ****\n" <<
  "\t\t\t\t\tm_is_set: " << m_is_set << "\n" <<
  "\t\t\t\t\tm_set: "; if (m_set) {m_set->print(); std::cout<<std::endl; } else { std::cout<<"nullptr\n"; }
  std::cout<< "\t\t\t\t\tm_cmd_word: " << m_cmd_word << "\n"
  << "\t\t\t\t\tm_arguments: "; if(m_arguments) { m_arguments->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  //std::cout<< "\tm_arg_count: " << m_arg_count << std::endl;
  std::cout<< "\t\t\t\t\t**** END S_COMMAND ****\n";

}

void S_command::make_set() {
  m_is_set = true;
}

//////////////////////////////////
// Command methods ///////////////
//////////////////////////////////
Command::~Command() {
  if (m_s_command) { delete m_s_command; }
  if (m_c_command) { delete m_c_command; }
}
void Command::print() const {
  std::cout<< "\n\t\t\t\t**** COMMAND ****\n";
  std::cout<< "\t\t\t\tm_s_command: ";
    if (m_s_command) { m_s_command->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  //if (m_c_command) { std::cout<< "\tm_c_command: "; m_c_command->print(); }
  std::cout<< "\t\t\t\t**** END COMMAND ****\n";
}

//////////////////////////////////
// Redirect methods //////////////
//////////////////////////////////

// DO NOT USE UNTIL ACTIONS ARE SET IN SHELL.Y
void Redirect::print() const {
  std::cout<< "\n\t\t\t**** REDIRECT ****\n" <<
  "\t\t\tm_type: " << m_type << std::endl <<
  "\t\t\tm_filename: " << m_filename << std::endl;
}

//////////////////////////////////
// Pipe_seq methods //////////////
//////////////////////////////////
Pipe_seq::~Pipe_seq() {
  if (m_pipe_seq) { delete m_pipe_seq; }
  if (m_command) { delete m_command; }
}

void Pipe_seq::print() const {
  std::cout<< "\n\t\t**** PIPE SEQUENCE ****\n";
  std::cout<< "\t\tm_pipe_seq: "; if (m_pipe_seq) { m_pipe_seq->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\t\tm_command: "; if (m_command) { m_command->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\t\t**** END PIPE SEQUENCE ****\n";
}

//////////////////////////////////
// Ccs methods ///////////////////
//////////////////////////////////
Ccs::~Ccs() {
  if (m_ccs) { delete m_ccs; }
  if (m_pipe_seq) { delete m_pipe_seq; }
  if (m_redirect) { delete m_redirect; }
}

void Ccs::print() const {
  std::cout<< "\n\t**** CCS ****\n" <<
  "\tm_ccs: "; if (m_ccs) { m_ccs->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\tm_pipe_seq: "; if (m_pipe_seq) { m_pipe_seq->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\tm_redirect: "; if (m_redirect) { m_redirect->print(); }
   else { std::cout<< "nullptr\n"; }
  std::cout<< "**** END CCS ****\n";
}

//////////////////////////////////
// Program methods ///////////////
//////////////////////////////////
Program::~Program() {
  if (m_ccs) { delete m_ccs; }
}

void Program::print() const {
  std::cout<< "\n**** PROGRAM ****\n";
  std::cout<< "m_ccs: "; if (m_ccs) { m_ccs->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "**** END PROGRAM ****\n";
}

void Program::set_bg() {
  m_background = true;
}
