// Shell implementation. Primary functions are run() and execute() which will run
//  through the whole process of executing a command (expansions etc)
//  Secondary functions are the built-in executers.

#include "Shell.h"
#include <cassert>
#include <unistd.h>
#include <cstring>

// Global variable, probably not going to be used
bool big_exit = 0;

// This is the big one; it contains the shell "loop" and sets up the input
// to be sent to yyparse(). Also takes care of deleting various allocated memories.
int Shell::run() {
  std::string in_string;
  extern Program *root;

  char *line_prefix = new char[256];
  line_prefix = getcwd( line_prefix, 256);
  if (!line_prefix) { return -1; } // cwd error

  do {
    // Handle empty commands ie lone carriage returns
    std::cout<< line_prefix << ": ";
    std::getline(std::cin, in_string);
  } while(in_string == "");

  char *str_copy = new char[in_string.size()+1];
  strcpy(str_copy, in_string.c_str());
  YY_BUFFER_STATE bp = yy_scan_string(str_copy);
  yyparse();
  Program *new_root = new Program(*root);
  int exit_status = execute(new_root);
  yy_delete_buffer(bp);
  delete[] line_prefix;
  delete[] str_copy;
  delete root;
  delete new_root;



  return exit_status;
}

int Shell::execute(Program *in_prog) {
  // while (in_prog->has_unescaped_chars) {
  //  in_prog->escape_chars();
  // }
  // while (in_prog->has_unexpanded_vars) {
  //  in_prog->expand_vars();
  //}
  // while (in_prog->has_unexpanded_source) {
  //  in_prog->expand_source();
  //}
  // while (!in_prog->ready_to_execute()) {
  //  in_prog->escape_chars();
  //  in_prog->expand_vars();
  //  in_prog->expand_source();
  //}
  // in_prog->perform_redirects();
  // in_prog->perform_pipes();

  int exit_status = 0;
  Ccs *current = in_prog->m_ccs;
  while (current != nullptr) { // this while loop runs once during simple execution
    std::string cmd_to_be_checked(current->m_pipe_seq->m_command->m_s_command->m_cmd_word);
    if (is_built_in(current->m_pipe_seq->m_command->m_s_command)) {
      exit_status = execute_built_in(current->m_pipe_seq->m_command->m_s_command,
                                     current->m_pipe_seq->m_command->m_s_command->m_arguments);
    } else {
      exit_status = execute_external(current->m_pipe_seq->m_command->m_s_command,
                                     current->m_pipe_seq->m_command->m_s_command->m_arguments);
    }
    current = current->m_ccs; // this will allow us to execute chains of commands
  }
  return exit_status;
}

bool Shell::is_built_in(S_command *in_com) {
  std::string local_cmd_word{};
  if (in_com) {
    local_cmd_word = in_com->m_cmd_word;
    if (   local_cmd_word == "cd"
        || local_cmd_word == "echo"
        || local_cmd_word == "pwd"
        || local_cmd_word == "source"
        || local_cmd_word == "set"
        || local_cmd_word == "exit")
        { return true; } else { return false; }
  }
  else {
    return false;
  }
}

bool Shell::_exit() {
  return shell_exit;
}

// Sorry this next function is a mess. It essentially checks if the
// input pointers are valid, checks if the cmd matches a built-in command,
// and executes accordingly. Returns -1 if an error occurs.
int Shell::execute_built_in(S_command *in_com, Arguments *in_arg) {
  std::string command_to_be_checked;
  if (in_com) { // always check your pointers :-)
    command_to_be_checked = in_com->m_cmd_word;
  }
  if (in_arg) {
    if (command_to_be_checked == "cd") {
      return execute_cd(in_com,in_arg);
    } else if (command_to_be_checked == "echo") {
      return execute_echo(in_com,in_arg);
    } else if (command_to_be_checked == "set") {
      return execute_set(in_com);
    } else if (command_to_be_checked == "exit") {
      return execute_exit(in_com);
    } else if (command_to_be_checked == "source") {
      return execute_source(in_com,in_arg);
    } else { return -1; } // error, not built-in
  } else {
    if (command_to_be_checked == "cd") {
      return execute_cd(in_com,nullptr);
    } else if (command_to_be_checked == "echo") {
      return execute_echo(in_com,nullptr);
    } else if (command_to_be_checked == "set") {
      return execute_set(in_com);
    } else if (command_to_be_checked == "source") {
      return execute_source(in_com,nullptr);
    } else if (command_to_be_checked == "pwd") {
      return execute_pwd(in_com);
    } else if (command_to_be_checked == "exit") {
      return execute_exit(in_com);
    }
  }
  return -1; // something went wrong
}

int Shell::execute_external(S_command *in_com, Arguments *in_arg) {
  return 0;
}

int Shell::execute_cd(const S_command* in_com, const Arguments* in_arg){
  assert(in_com->m_cmd_word == "cd");
  int result{};
  if (!in_arg) {
    result = chdir("~");
    return result;
  }
  if (in_arg->m_arg_count != 1) {
    return -1; // error, too many arguments
  } else {
    std::string cd_arg(in_arg->m_arguments[0]);
    result = chdir(cd_arg.c_str());
    return result;
  }
}

int Shell::execute_pwd(const S_command* in_com){
  assert(in_com->m_cmd_word == "pwd");
  char *temp = new char[512]; // long-ass char string to handle most cwd paths
  temp = getcwd(temp, 512);
  if (!temp) { return -1; } // error getting cwd
  std::cout<< temp << std::endl; // for REDIRECT, and PIPES, make sure to
                                // change std:cout to whatever stream is being redirected to
  delete[] temp;
  return 0;
}

int Shell::execute_source(const S_command*, const Arguments*){
  std::cout<< "[][][][] SOURCE NOT IMPLEMENTED [][][][]" << std::endl;
  return 0;
}

int Shell::execute_exit(const S_command* in_com){
  assert(in_com->m_cmd_word == "exit");
  shell_exit = true;
  return 0;
}

int Shell::execute_echo(const S_command* in_com, const Arguments* in_arg){
  assert(in_com->m_cmd_word == "echo");
  if (!in_arg) {
    std::cout<<std::endl; // print empty line
    return 0;
  }
  std::string output{};
  for (int i = 0; i < in_arg->m_arg_count; i++) {
    output += in_arg->m_arguments[i] + " ";
  }
  output.pop_back();
  std::cout<< output << std::endl;
  return 0;
}

int Shell::execute_set(const S_command* in_com){
  assert(in_com->m_is_set);
  assert(in_com->m_cmd_word == "set");
  if (in_com->m_set) {
    m_symbol_table.insert( {in_com->m_set->m_varname, in_com->m_set->m_value} );
    return 0;
  } else { return -1; } // something went wrong
}
