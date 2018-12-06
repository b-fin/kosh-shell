// Implementation file for AST structure

#include "AST.h"

//////////////////////////////////
// Arguments methods /////////////
//////////////////////////////////

Arguments& Arguments::operator=(const Arguments &source) {
  if (this == &source) {
    return *this;
  }
  m_arguments.clear();
  m_arg_count = 0;
  for (int i=0; i<source.m_arg_count; i++) {
    std::string new_arg(source.m_arguments[i]);
    m_arguments.push_back(new_arg);
    m_arg_count++;
  }
  return *this;

}

void Arguments::add_argument(char *in_str) {
  std::string new_arg(in_str);
  m_arguments.push_back(new_arg);
  m_arg_count++;
  free(in_str);
}

// Debug method
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

// Deleted for now //
/********************************************************************
void Arguments::remove_quotes() {
  for (auto x : m_arguments) {
    if (x.front() == '\"' && x.back() == '\"') {
      unsigned temp_size = x.size();
      std::string temp_string = x.substr(1,temp_size-2);
      x = temp_string;
    }
  }
}
************************************************************************/

// Takes one parameter, the Shell symbol table (ugly type name)
// Returns:
//  - true if all arguments that need expanding were successfully expanded.
//  - false otherwise.
bool Arguments::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  bool status = false;
  int position = 0;
  for (auto& x : m_arguments) {
    if (word_can_be_expanded(x)) {
      if(!word_expand_arg(in_s_t, m_arguments, x, position)) {
        // variable could not be expanded
        status = false;
        std::cout<< "Error: in Arguments::expand_vars(), argument that could be expanded was not\n";
        break;
      } else {
          status = true;
        }
    }
    position++;
  }
  return status;
}

// Returns:
//  - true if any argument needs expanding i.e. contains a '$'
//  - false otherwise
bool Arguments::needs_expanding() const {
  bool status = false;
  for (auto x : m_arguments) {
    if (word_can_be_expanded(x)) {
      status = true;
    }
  }
  return status;
}
//////////////////////////////////
// Set methods ///////////////////
//////////////////////////////////

Set& Set::operator=(const Set& source) {
  if (this == &source) {
    return *this;
  }
  m_varname = source.m_varname;
  m_value = source.m_value;
  return *this;
}

// Debug method
void Set::print() const {
  std::cout<< "**** SET ****\n"<<
  "\tVarname: " << m_varname << std::endl <<
  "\tValue: " << m_value << std::endl;
  std::cout<< "**** END SET ****\n";
}

// Deleted for now //
/*************************************************
void Set::remove_quotes() {
  if (m_value.front() == '\"' && m_value.back() == '\"') {
    unsigned temp_size = m_value.size();
    std::string new_string = m_value.substr(1,temp_size-2);
    m_value = new_string;
  }
}
**************************************************/

// First parameter is Shell symbol table.
// Returns true if value was expanded successfully, false otherwise.
bool Set::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  int exit_status = false;
  if (word_can_be_expanded(m_value)) {
    std::string tmp = word_expand_value(in_s_t, m_value);
    if (tmp != "") {
      m_value = tmp;
      exit_status = true;
    } else {
        std::cout<< "ERROR: in Set::expand_vars, word_expand_value returned false\n";
        exit_status = false;
      }
  } else {
    std::cout<< "ERROR: in Set::expand_vars, word_can_be_expanded() returned false\n";
  }
  return exit_status;
}

bool Set::needs_expanding() const {
  if (word_can_be_expanded(m_value)) {
    return true;
  } else {
    return false;
  }
}

//////////////////////////////////
// S_command methods /////////////
//////////////////////////////////

S_command& S_command::operator=(const S_command& source) {
  //std::cout<< "S_COMMAND assignment op called\n";
  if (this == &source) {
    return *this;
  }
  m_cmd_word = source.m_cmd_word;
  if (m_set) {
    delete m_set;
  }
  if (m_arguments) {
    delete m_arguments;
  }
  if (source.m_set) {
    m_set = new Set(*source.m_set);
    m_is_set = true;
  }
  else if (source.m_arguments) {
    m_arguments = new Arguments(*source.m_arguments);
  }
  return *this;
}

S_command::~S_command() {
  if (m_set) { delete m_set; }
  if (m_arguments) { delete m_arguments; }
}

// Debug method
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

// Returns true if words that need to be expanded were expanded successfully
bool S_command::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  bool status;
  bool args_worked;
  bool cmd_worked;

  if (m_set) {
    status = m_set->expand_vars(in_s_t);
  } else {
      // first, check if both args and cmdword need expanding, and
      // have status be true ONLY if both expand successfully.
      if (cmd_word_needs_expanding() && args_need_expanding()) {
        std::string tmp = word_expand_value(in_s_t,m_cmd_word);
        if (tmp == "") {
          cmd_worked = false;
        } else {
          cmd_worked = true;
          m_cmd_word = tmp;
        }
        args_worked = m_arguments->expand_vars(in_s_t);
        status = args_worked && cmd_worked;
      } else {
          // either cmd_word needs expanding or args do. find out which one
          // and expand it.
          if (cmd_word_needs_expanding()) {
            std::string tmp = word_expand_value(in_s_t,m_cmd_word);
            if (tmp == "") {
              status = false;
            } else {
              status = true;
              m_cmd_word = tmp;
            }
          } else if (args_need_expanding()) {
              status = m_arguments->expand_vars(in_s_t);
          } else {
              // we shouldn't get here so something went wrong.
              std::cout<< "ERROR: in S_command::expand_vars(), we got to the catch-all else\n";
              status = false;
          }
        }
  }
  return status;
}


// Returns true if either:
// 1. m_set exists and needs expanding or
// 2. either cmd_word needs expanding or arguments need expanding
bool S_command::needs_expanding() const {
  bool status;
  if (m_set) {
    status = m_set->needs_expanding();
  } else {
      // args_need_expanding checks for m_arguments so we don't have to
      if(args_need_expanding() || cmd_word_needs_expanding()) {
        status = true;
      } else {
          status = false;
      }
  }
  return status;
}

bool S_command::cmd_word_needs_expanding() const {
  return (word_can_be_expanded(m_cmd_word));
}

bool S_command::args_need_expanding() const {
  bool status;
  (m_arguments) ? status=m_arguments->needs_expanding() : status=false;
  return status;
}

//////////////////////////////////
// Command methods ///////////////
//////////////////////////////////

Command& Command::operator=(const Command& source) {
  //std::cout<< "COMMAND assignment op called\n";
  if (this == &source) {
    return *this;
  }
  if (m_s_command) {
    delete m_s_command;
  }
  if (m_c_command) {
    delete m_c_command;
  }
  if (source.m_s_command) {
    m_s_command = new S_command(*source.m_s_command);
  }
   else if (source.m_c_command) {
    m_c_command = new C_command(*source.m_c_command);
  }
  return *this;
}

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

bool Command::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  int exit_status = true;
  if (m_s_command) {
    exit_status = m_s_command->expand_vars(in_s_t);
  }
  return exit_status;
}

bool Command::needs_expanding() const {
  bool exit_status = false;
  if (m_s_command) {
    exit_status = m_s_command->needs_expanding();
  }
  return exit_status;
}
//////////////////////////////////
// Redirect methods //////////////
//////////////////////////////////

Redirect& Redirect::operator=(const Redirect& source) {
  if (this == &source) {
    return *this;
  }
  m_type = source.m_type;
  m_filename = source.m_filename;
  return *this;
}

// DO NOT USE UNTIL ACTIONS ARE SET IN SHELL.Y
void Redirect::print() const {
  std::cout<< "\n\t\t\t**** REDIRECT ****\n" <<
  "\t\t\tm_type: " << m_type << std::endl <<
  "\t\t\tm_filename: " << m_filename << std::endl;
}

//////////////////////////////////
// Pipe_seq methods //////////////
//////////////////////////////////

Pipe_seq& Pipe_seq::operator=(const Pipe_seq& source) {
  //std::cout<< "PIPE_SEQ assignment op called\n";
  if (this == &source){
    return *this;
  }
  if (m_pipe_seq) {
    delete m_pipe_seq;
  }
  if (m_command) {
    delete m_command;
  }
  if (source.m_pipe_seq) {
    m_pipe_seq = new Pipe_seq(*source.m_pipe_seq);
  }
  if (source.m_command) {
    m_command = new Command(*source.m_command);
  }
  return *this;
}

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

bool Pipe_seq::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  int exit_status = true;
  if (m_command) {
    exit_status = m_command->expand_vars(in_s_t);
  }
  return exit_status;
}

bool Pipe_seq::needs_expanding() const {
  bool exit_status = false;
  if (m_command) {
    exit_status = m_command->needs_expanding();
  }
  return exit_status;
}
//////////////////////////////////
// Ccs methods ///////////////////
//////////////////////////////////

Ccs& Ccs::operator=(const Ccs& source) {
  //std::cout<< "CCS assignment op called\n";
  if (this == &source) {
    return *this;
  }
  if (m_ccs) {
    delete m_ccs;
  }
  if (m_pipe_seq) {
    delete m_pipe_seq;
  }
  if (m_redirect) {
    delete m_redirect;
  }
  if (source.m_ccs) {
    m_ccs = new Ccs(*source.m_ccs);
  }
  if (source.m_pipe_seq) {
    m_pipe_seq = new Pipe_seq(*source.m_pipe_seq);
  }
  if (source.m_redirect) {
    m_redirect = new Redirect(*source.m_redirect);
  }
  return *this;
}

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

bool Ccs::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  Pipe_seq *cur_pipe = m_pipe_seq;
  // as before, default exit is bad. only way it can be good is if while loop
  // enters and changes it to a good one.
  int exit_status=true;
  while (cur_pipe != nullptr) {
    exit_status = cur_pipe->expand_vars(in_s_t);
    cur_pipe = cur_pipe->m_pipe_seq;
  }
  return exit_status;
}

bool Ccs::needs_expanding() const {
  bool exit_status = false;
  Pipe_seq *cur_pipe = m_pipe_seq;
  while (cur_pipe != nullptr) {
    exit_status = cur_pipe->needs_expanding();
    cur_pipe = cur_pipe->m_pipe_seq;
  }
  return exit_status;
}

//////////////////////////////////
// Program methods ///////////////
//////////////////////////////////

Program& Program::operator=(const Program& source) {
  //std::cout<< "PROGRAM assignment op called\n";
  if (this == &source) {
    return *this;
  }
  if (m_ccs) {
    delete m_ccs;
  }
  if (source.m_ccs) {
    m_ccs = new Ccs(*source.m_ccs);
  }
  m_background = source.m_background;
  return *this;
}

Program::~Program() {
  if (m_ccs) { delete m_ccs; }
}

void Program::print() const {
  std::cout<< "\n**** PROGRAM ****\n";
  std::cout<< "m_ccs: "; if (m_ccs) { m_ccs->print(); std::cout<<std::endl; }
    else { std::cout<< "nullptr\n"; }
  std::cout<< "**** END PROGRAM ****\n";
}


bool Program::expand_vars(const std::unordered_map<std::string,std::string>& in_s_t) {
  // idea: have the ccs while loop in here, just call ccs::expand etc
  Ccs *cur_ccs = m_ccs;
  // default bad value; only way it can report good is if the while loop enters
  // and the other calls go well
  int exit_status=true;
  while(cur_ccs != nullptr) {
    exit_status = cur_ccs->expand_vars(in_s_t);
    cur_ccs = cur_ccs->m_ccs;
  }
  return exit_status;
}

bool Program::needs_expanding() const {
  // default is 0 because m_ccs might fail
  bool exit_status = false;
  Ccs *cur_ccs = m_ccs;
  while(cur_ccs != nullptr) {
    exit_status = cur_ccs->needs_expanding();
    cur_ccs = cur_ccs->m_ccs;
  }
  return exit_status;
}

void Program::do_nothing() const {
  ;
}
