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
  std::cout<< "**** ARGUMENTS ****\n";
  for (int i=0; i<m_arg_count; i++) {
    std::cout<< "\t#" << i << ": " << m_arguments[i] << std::endl;
  }
}

//////////////////////////////////
// Set methods ///////////////////
//////////////////////////////////

void Set::print() const {
  std::cout<< "**** SET ****\n"<<
  "\tVarname: " << m_varname << std::endl <<
  "\tValue: " << m_value << std::endl;
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
  std::cout<< "**** S_COMMAND ****\n" <<
  "\tm_is_set: " << m_is_set << "\n" <<
  "\tm_set: "; if (m_set) {m_set->print(); } else { std::cout<<"nullptr\n"; }
  std::cout<< "\tm_cmd_word: " << m_cmd_word << "\n"
  << "\tm_arguments: "; if(m_arguments) { m_arguments->print(); }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\tm_arg_count: " << m_arg_count << std::endl;

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
  std::cout<< "**** COMMAND ****\n";
  std::cout<< "\tm_s_command: ";
    if (m_s_command) { m_s_command->print(); }
    else { std::cout<< "nullptr\n"; }
  //if (m_c_command) { std::cout<< "\tm_c_command: "; m_c_command->print(); }
}

//////////////////////////////////
// Redirect methods //////////////
//////////////////////////////////

void Redirect::print() const {
  std::cout<< "**** REDIRECT ****\n" <<
  "\tm_type: " << m_type << std::endl <<
  "\tm_filename: " << m_filename << std::endl;
}

//////////////////////////////////
// Pipe_seq methods //////////////
//////////////////////////////////
Pipe_seq::~Pipe_seq() {
  if (m_pipe_seq) { delete m_pipe_seq; }
  if (m_command) { delete m_command; }
}

void Pipe_seq::print() const {
  std::cout<< "**** PIPE SEQUENCE ****\n";
  std::cout<< "m_pipe_seq: "; if (m_pipe_seq) { m_pipe_seq->print(); }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "m_command: "; if (m_command) { m_command->print(); }
    else { std::cout<< "nullptr\n"; }
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
  std::cout<< "**** CCS ****\n" <<
  "\tm_pipe_seq: "; if (m_pipe_seq) { m_pipe_seq->print(); }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "\tm_redirect: "; if (m_redirect) { m_redirect->print(); }
   else { std::cout<< "nullptr\n"; }
}

//////////////////////////////////
// Program methods ///////////////
//////////////////////////////////
Program::~Program() {
  if (m_ccs) { delete m_ccs; }
}

void Program::print() const {
  std::cout<< "**** PROGRAM ****\n";
  std::cout<< "m_ccs: "; if (m_ccs) { m_ccs->print(); }
    else { std::cout<< "nullptr\n"; }
}

void Program::set_bg() {
  m_background = true;
}
