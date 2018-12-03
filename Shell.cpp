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
  print_symbol_table();
  delete[] line_prefix;
  delete[] str_copy;
  delete root;
  delete new_root;



  return exit_status;
}
// Other big function. Performs expansions and (eventually) redirections,
//  also makes calls to the various sub-execution methods eg execute_cd,
//  execute_internal.
int Shell::execute(Program *in_prog) {
   while (!in_prog->ready_to_execute()) {
  //  in_prog->escape_chars(); TODO
    if (expand_vars(in_prog) != 0) { return -1; };
  //  in_prog->expand_source(); TODO
  }
  // in_prog->perform_redirects(); TODO
  // in_prog->perform_pipes(); TODO

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
    std::cout<< "ERROR: cd takes 1 argument\n";
    return -1; // error, too many arguments
  } else {
    std::string cd_arg(in_arg->m_arguments[0]);
    result = chdir(cd_arg.c_str());
    if (result != 0) {
      std::cout<< "ERROR: " << cd_arg << " not valid directory\n";
    }
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
  //std::cout<< "We are in echo\n";
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
    // check if varname is already in symbol table; if so, overwrite it
    if (m_symbol_table.count(in_com->m_set->m_varname) == 1) {
      m_symbol_table[in_com->m_set->m_varname] = in_com->m_set->m_value;
      return 0;
    }
    else {
    m_symbol_table.insert( {in_com->m_set->m_varname, in_com->m_set->m_value} );
    return 0;
    }
  } else {
    std::cout<< "ERROR: Could not set variable\n";
    return -1;
    } // something went wrong
}

void Shell::print_symbol_table() const {
  std::cout<< "SYMBOL TABLE CONTENTS: " << std::endl;
  for (auto& x : m_symbol_table) {
    std::cout<< x.first<< ": "<< x.second << std::endl;
  }
}

int Shell::expand_vars(Program *in_prog) {
  //std::cout<< "we are in expand_vars\n";
  Ccs *current_ccs;
  Pipe_seq *current_pipe;
  std::string ostring = "";
  if (in_prog->m_ccs) {
    current_ccs = in_prog->m_ccs;
  } else { std::cout<< "ERROR: No m_ccs node detected\n"; return -1; } // error, no ccs node
  // Traverse the tree, check for instances of '$' within strings.
  while (current_ccs != nullptr) {
    if (current_ccs->m_pipe_seq) {
      current_pipe = current_ccs->m_pipe_seq;
    } else { std::cout<< "ERROR: no m_pipe_seq node detected\n"; return -1; } // error, no pipe_seq node

    while (current_pipe != nullptr) {
      // check for set command; if so, check its varname and value fields for '$' and deal with them
      if (current_pipe->m_command && current_pipe->m_command->m_s_command && current_pipe->m_command->m_s_command->m_set) {
        if (current_pipe->m_command->m_s_command->m_set->m_varname.find('$') != std::string::npos) {
          //std::cout<< "In expand_vars, we are in the first find($) branch\n";
          if (try_expand_vars(current_pipe->m_command->m_s_command->m_set->m_varname, ostring)) {
            current_pipe->m_command->m_s_command->m_set->m_varname = ostring;
            return 0;
          }
        } else if (current_pipe->m_command->m_s_command->m_set->m_value.find('$') != std::string::npos) {
          //std::cout<< "In expand_vars, we are in the second find($) branch\n";
          if (try_expand_vars(current_pipe->m_command->m_s_command->m_set->m_value, ostring)) {
            current_pipe->m_command->m_s_command->m_set->m_value = ostring;
            return 0;
          }
        }
      }
      // no set command; check command word and arguments for '$' and deal with them
      if (current_pipe->m_command && current_pipe->m_command->m_s_command) {
        if (current_pipe->m_command->m_s_command->m_cmd_word.find('$') != std::string::npos) {
          //std::cout<< "In expand_vars, we are in the third find($) branch\n";
          if (try_expand_vars(current_pipe->m_command->m_s_command->m_cmd_word, ostring)) {
            current_pipe->m_command->m_s_command->m_cmd_word = ostring;
            return 0;
          }
        }
        // now try arguments
        if (current_pipe->m_command->m_s_command->m_arguments) {
          for (auto& x : current_pipe->m_command->m_s_command->m_arguments->m_arguments) {
            if (x.find('$') != std::string::npos) {
              if (try_expand_vars(x, ostring)) {
                x = ostring;
                return 0;
              }
            }
          }
        }
      }
      // we got here, which means no command or argument in this pipe sequence has a '$'.
      // so, we advance the current_pipe.
      current_pipe = current_pipe->m_pipe_seq;
    } // end inner while

    current_ccs = current_ccs->m_ccs;
  } // end outer while
  // we made it out, so there must be an error?
  std::cout<< "ERROR: Variable not set" <<std::endl;
  return -1; // Variable likely not set
}

bool Shell::try_expand_vars(std::string in_str, std::string& ostring) {
  //std::cout<< "We are in try_expand_vars\n";
  assert(in_str.find('$') != std::string::npos);
  int pos = in_str.find('$');
  // check for bad case: '$' at end of string
  if (in_str.back() == '$') {
    std::cout<< "ERROR: '$' detected at end of string " << in_str << std::endl;
    return false;
  }
  // check for good case: '$' at beginning of string
  if (in_str.front() == '$') {
    // check if the substring in_str[1:size-1] matches anything in the symbol table

    // temp_string becomes a copy of in_str without the leading '$'
    std::string temp_string = in_str.substr(1,std::string::npos);
    // we then go through the symbol table, and try to match the temp_string with
    // anything in the symbol table. If it matches, we set the out parameter ostring
    // to the value associated with the key and return true.
    for (auto& x : m_symbol_table) {
      if (temp_string == x.first) {
        ostring = x.second;
        return true;
      }
    }
  }
  // middle case: '$' occurs in between the beginning and end of string
  // this could get hairy: what we want to do is take the string,
  // get the substring from pos+1 to the end, and check it against the symbol table
  // keys. if it matches, we append the key's value to the beginning of the old string (up until the $)
  // we then follow the out-parameter procedure as above
  std::string temp_string = in_str.substr(pos+1, std::string::npos);
  std::string temp_prefix = in_str.substr(0,pos);
  assert(temp_string.size() >= 1); // ensure we have a non-empty temp_string to check
  for (auto& x : m_symbol_table) {
    if ( temp_string == x.first) {
      ostring = temp_prefix + x.second;
      return true;
    }
  }
  // We somehow reached the end of the function, which means something went wrong
  std::cout << "WARNING: Control reached end of try_expand_vars(i,o) without returning true\n";
  return false;
}
