// Shell implementation. Primary functions are run() and execute() which will run
//  through the whole process of executing a command (expansions etc)
//  Secondary functions are the built-in executers and execute_external


/****************************************************************
 *   TO DO: -remove asserts   [done]                            *
 *          -fix function logic / single returns  [done         *
 *          -get pipes working                                  *
 *          -get ';'s working                                   *
 *          -get commands subs working                          *
 *          -get '\'s working                                   *
 *          -get wildcards working                              *
 ****************************************************************/

#include "Shell.h"
#include <cassert>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <fstream>

#define KOSH_DEBUG_PRINT false


// This is the big one; it sets up the input to be sent to yyparse()
// and stores a personal copy of the AST built by yyparse() for execution.
// Returns 0 on successful execution, nonzero otherwise.
int Shell::run() {
  ExecStatus status = EXEC_IN_PROG;
  int exit_status;

  std::string in_string;
  Program *sh_root = new Program(nullptr);
  char *line_prefix = new char[256];
  line_prefix = getcwd( line_prefix, 256);

  if (!line_prefix) {
    std::cout<< "Error getting cwd prefix\n";
    status = EXEC_ERR_OTHER;
  } else {
      // Handles empty lines, ie when the user only presses enter
      do {
        std::cout<< line_prefix << ": ";
        std::getline(std::cin, in_string);
      } while(in_string == "");

      // allocate a string and buffer for Bison to parse
      char *str_copy = new char[in_string.size()+1];
      strcpy(str_copy, in_string.c_str());
      YY_BUFFER_STATE bp = yy_scan_string(str_copy);
      yyparse(*sh_root);

      // copy the root that Bison modified so we can do multiple successive parses
      // without losing AST information.
      Program *new_root = new Program(*sh_root);
      Program& prog_ref = *new_root;

      exit_status = execute(prog_ref);
      // Debug print
      //std::cout<< "Exit status is: " << exit_status << std::endl;

      // free memory allocated in this scope
      yy_delete_buffer(bp);
      print_symbol_table();
      delete[] str_copy;
      delete new_root;
    }
  // free memory allocated in the function scope
  delete[] line_prefix;
  delete sh_root;

  status = static_cast<ExecStatus>(exit_status);
  return status;
}

// Other big function. Performs expansions and (eventually) redirections,
//  also makes calls to the various sub-execution methods eg execute_cd,
//  execute_internal.
int Shell::execute(Program& in_prog) {
  ExecStatus status = EXEC_IN_PROG;
  int exit_status = 0;

  bool expanded=true;
  if (in_prog.needs_expanding()) {
    expanded = expand_vars(in_prog);
    // debug print
    std::cout<< "expanded is: " << expanded << std::endl;
    if(!expanded) {
      std::cout<< "ERROR: variable expansion failed\n";
      status = EXEC_ERR_VAR_EXP;
    }
  }

  Ccs *current;
  if (in_prog.m_ccs) {
    current = in_prog.m_ccs;
  } else {
      // debug print
      //std::cout<< "EXECUTE: no m_ccs member\n";
      status = EXEC_ERR_MISSING_NODE;
    }
  if (status == EXEC_IN_PROG) {
    // this while loop runs once during simple execution (ie no chains or pipes)
    while (current != nullptr) {
      std::string cmd_to_be_checked(current->m_pipe_seq->m_command->m_s_command->m_cmd_word);
      if (is_built_in(*current->m_pipe_seq->m_command->m_s_command)) {
        exit_status = execute_built_in(*current->m_pipe_seq->m_command->m_s_command);
        if (exit_status == 0) status = EXEC_SUCCESS; else status = EXEC_FAIL;
      } else {
        exit_status = execute_external(*current->m_pipe_seq->m_command->m_s_command);
        if (exit_status == 0) status = EXEC_SUCCESS; else status = EXEC_FAIL;
      }
      // this will allow us to execute chains of commands
      current = current->m_ccs;
    }
  } else {
    status = EXEC_FAIL;
  }


  return status;
}

// Returns true if the parsed command matches one of the six built-ins,
// false otherwise.
bool Shell::is_built_in(S_command& in_com) {
  bool status = false;
  if (in_com.m_cmd_word == "cd"  || in_com.m_cmd_word == "echo" || in_com.m_cmd_word == "pwd"
  || in_com.m_cmd_word == "source" || in_com.m_cmd_word == "set"  || in_com.m_cmd_word == "exit"    )
   {
        status = true;
  }
  else {
    status = false;
  }
  return status;
}


// Checks the parsed command with the 6 built-ins and calls the
// respective executer if there's a match.
// Returns the exit code of the called executer:
//  - Zero on success
//  - Nonzero otherwise
int Shell::execute_built_in(S_command &in_com) {
  ExecStatus status = EXEC_IN_PROG;
    if (in_com.m_cmd_word == "cd") {
      status = static_cast<ExecStatus>(execute_cd(in_com));
    } else if (in_com.m_cmd_word == "echo") {
      status = static_cast<ExecStatus>(execute_echo(in_com));
    } else if (in_com.m_cmd_word == "set") {
      status = static_cast<ExecStatus>(execute_set(in_com));
    } else if (in_com.m_cmd_word == "source") {
      status = static_cast<ExecStatus>(execute_source(in_com));
    } else if (in_com.m_cmd_word == "pwd") {
      status = static_cast<ExecStatus>(execute_pwd(in_com));
    } else if (in_com.m_cmd_word == "exit"){
      status = static_cast<ExecStatus>(execute_exit(in_com));
    } else {
        if (KOSH_DEBUG_PRINT) {
          std::cout<< "ERROR: something went wrong in execute_built_in()\n";
        }
        status = EXEC_FAIL;
      }
    return status;
}

// This function executes commands other than the 6 built-ins.
// It does this by calling fork() and exec().
// Returns:
//  - Zero on successful execution
//  - Nonzero otherwise
int Shell::execute_external(S_command &in_com) {
  ExecStatus status = EXEC_IN_PROG;

  // Prepare argv[], which must be of the form:
  // [cmd_word]|[arg1]|[arg2]\...|[argn]|[NULL]
  // in order for execvp to take it (it's very picky when all the GCC warnings
  // are enabled.)
  int tmp_size = 2;
  bool args = in_com.has_args();

  if(args) {
    tmp_size = in_com.m_arguments->m_arg_count+2;
  }

  char **argv = new char*[tmp_size];

  if(args) {
    for(int i=1; i<=in_com.m_arguments->m_arg_count; i++) {
      argv[i] = strdup(in_com.m_arguments->m_arguments[i-1].c_str());
    }
  }
  // at this point, if there aren't any arguments, then argv[]
  // contains the cmd_word at index 0 and NULL at index 1.
  // otherwise, cmd_word at index 0, arguments until index size-2, then NULL.
  argv[0] = strdup(in_com.m_cmd_word.c_str());
  argv[tmp_size-1] = NULL;

  // Actually begin fork-exec sequence
  pid_t cpid, w;
  int wstatus;
  cpid = fork();
  if(cpid == 0) {
    // we are in the child process
    if ( execvp(in_com.m_cmd_word.c_str(), const_cast<char *const(*)>(argv)) == -1) {
      std::cout<< "ERROR: error executing command (exec)\n";
      // we need to exit forcefully or else we would have two process of KOSH
      // running.
      exit(-1);
    }
  } else if (cpid < 0) {
      std::cout<< "ERROR: error executing command (fork)\n";
      status = EXEC_FAIL;
  } else {
      // parent process
      do {
        w = waitpid(cpid,&wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
          std::cout<< "ERROR: error executing command (wait)\n";
          status = EXEC_FAIL;
        }
      } while(!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    }
  // FREE/DELETE
  for(int i=0; i<tmp_size; i++) {
    free(argv[i]);
  }
  delete[] argv;
  return status;
}

// Returns:
//  - Zero on successful execution.
//  - Nonzero otherwise.
int Shell::execute_cd(const S_command& in_com){
  ExecStatus status = EXEC_IN_PROG;
  if (!in_com.has_args()) {
    status = static_cast<ExecStatus>(chdir(getenv("HOME")));
    if (status != EXEC_SUCCESS) status = EXEC_FAIL;
  } else {
      // in_com has arguments
      if (in_com.m_arguments->m_arg_count != 1) {
        std::cout<< "ERROR: cd takes 1 argument, more given\n";
        status = EXEC_FAIL;
    } else {
        std::string cd_arg(in_com.m_arguments->m_arguments[0]);
        status = static_cast<ExecStatus>(chdir(cd_arg.c_str()));
        if (status != EXEC_SUCCESS) status = EXEC_FAIL;
      }
  }
  return status;
}

// Returns:
//  - Zero on successful execution.
//  - Nonzero otherwise.
int Shell::execute_pwd(const S_command& in_com){
  // remove assert after debugging
  assert(in_com.m_cmd_word == "pwd");
  ExecStatus status = EXEC_IN_PROG;
  char *temp = new char[512]; // long-ass char string to handle any cwd paths
  temp = getcwd(temp, 512);
  if (!temp) {
    status = EXEC_FAIL;
  } else {
      status = EXEC_SUCCESS;
      std::cout<< temp << std::endl;
    }
  delete[] temp;
  return status;
}

// Returns:
//  - Zero on successful execution.
//  - Nonzero otherwise.
// This one checks the local directory for the file named in the argument node,
// and tries to open it.
int Shell::execute_source(const S_command& in_com){
  ExecStatus status = EXEC_IN_PROG;
  std::ifstream infile;
  // arg check; if has_args fails, or there is not exacty one argument,
  // jump to status = EXEC_FAIL below
  if ( !(in_com.has_args()==false || in_com.m_arguments->m_arg_count != 1)) {
    // good, continue with execution

    // prepare a char[] containing the PWD
    std::string filename {in_com.m_arguments->m_arguments[0]};
    std::string prefix;
    char *prefix_c = new char[256];
    getcwd(prefix_c,256);
    if (prefix_c) {
      // good, continue with execution
      // attach the PWD string to the filename argument and try to open
      prefix.assign(prefix_c);
      std::string prefix_plus_filename = prefix + "/" + filename;
      infile.open(prefix_plus_filename.c_str());
      if (infile.fail()) {
        // ie file not found in local dir or some opening error occurred.
        std::cout<< "WARNING: error opening file\n";
        infile.clear();
        infile.close();
        if ( ! search_path_for_file(filename).empty()) {
          // file found in PATH, try to open
          infile.open(search_path_for_file(filename).c_str());
          if (infile.fail()) {
            infile.clear();
            infile.close();
          }
        }
      }
    }
    delete[] prefix_c;
  }
  // at this point, either something went wrong or something went right.
  // we assign EXEC_FAIL because the only way it can be changed henceforth
  // is if the file opened, in which case status becomes the result of
  // executing lines of the file. (a consequence is that exit status of
  // source is that of the last executed command, if no errors, or of the first
  // command that fails.)
  status = EXEC_FAIL;

  if (infile.is_open()) {
    std::string line;
    int count = 0;
    while (getline(infile,line)) {
      count++;
      // set up yyparse
      Program *src_root = new Program(nullptr);
      char *str_copy = new char[line.size()+1];
      strcpy(str_copy, line.c_str());
      YY_BUFFER_STATE bp = yy_scan_string(str_copy);

      // parse, fill up src_root
      yyparse(*src_root);

      // use src_ref to execute the AST and receive a status
      Program *source_root = new Program(*src_root);
      Program& source_ref = *source_root;
      status = static_cast<ExecStatus>(execute(source_ref));

      // break down yyparse
      delete[] str_copy;
      yy_delete_buffer(bp);
      delete src_root;
      delete source_root;
    }
    infile.clear();
    infile.close();
  }
  return status;
}


// Returns 0
int Shell::execute_exit(const S_command& in_com) {
  shell_exit = true;
  return 0;
}

int Shell::execute_echo(const S_command& in_com) {
  ExecStatus status = EXEC_IN_PROG;
  if (!in_com.has_args()) {
    // print empty line
    std::cout<<std::endl;
    status = EXEC_SUCCESS;
  } else {
      // else: we have an arguments node
      std::string output{};
      for (int i = 0; i < in_com.m_arguments->m_arg_count; i++) {
        output += in_com.m_arguments->m_arguments[i] + " ";
      }
      // remove trailing space
      output.pop_back();
      std::cout<< output << std::endl;
      status = EXEC_SUCCESS; // guess it's hard to fail an echo?
  }
  return status;
}

// Returns:
//  - Zero on successful variable re/setting
//  - Nonzero otherwise
int Shell::execute_set(const S_command& in_com){
  ExecStatus status = EXEC_IN_PROG;
  // defensive guard
  if ( !(in_com.m_is_set && (in_com.m_cmd_word == "set"))) {
    status = EXEC_FAIL;

  // check if varname is already in symbol table; if so, overwrite it
  } else if (m_symbol_table.count(in_com.m_set->m_varname) == 1) {
      m_symbol_table[in_com.m_set->m_varname] = in_com.m_set->m_value;
      status = EXEC_SUCCESS;
  } else if (m_symbol_table.count(in_com.m_set->m_varname) == 0) {
      // insert into table as new element
      m_symbol_table.insert( {in_com.m_set->m_varname, in_com.m_set->m_value} );
      status = EXEC_SUCCESS;
    } else {
      // something went wrong
      status = EXEC_FAIL;
      std::cout<< "ERROR: Could not set variable\n";
    }
  return status;
}


// Debug method, ignore
void Shell::print_symbol_table() const {
  if (KOSH_DEBUG_PRINT) {
    std::cout<< "SYMBOL TABLE CONTENTS: " << std::endl;
    for (auto& x : m_symbol_table) {
      std::cout<< x.first<< ": "<< x.second << std::endl;
    }
  }
}

// Initiates a telescoping series of calls down the AST.
// Returns:
//  - true if successful expansion
//  - false otherwise
bool Shell::expand_vars(Program& in_prog) {
  int exit_status;
  exit_status = in_prog.expand_vars(m_symbol_table);
  return exit_status;
}

// Returns:
//  - A string matching the first directoy in the PATH variable
//    that matches the in_str parameter (typically a filename argument to
//    source).
//  - "" (empty string) otherwise (no match found, etc)
std::string Shell::search_path_for_file(std::string in_str) const {
  std::string ret_str = "";
  std::string total_path;
  total_path.assign(getenv("PATH"));
  std::vector<std::string> paths = split(total_path,":");
  if (paths.empty()) {
    // something went wrong
  } else {
      // if non empty, we can begin to try and open the file with each of the PATH
      // prefixes. if one works, we send the whole path + filename to source. otherwise,
      // we send an empty string.
      std::ifstream tst_file;
      std::string prefix{};
      std::string prefix_plus_filename;
      for (int i=0; static_cast<size_t>(i)<paths.size(); i++) {
        prefix = paths[i];
        prefix_plus_filename = prefix + "/" + in_str;
        tst_file.open(prefix_plus_filename);
        if(!tst_file.fail()) {
          tst_file.clear();
          tst_file.close();
          ret_str = prefix_plus_filename;
          break;
        } else {
          // tst_file is still open (?) I'm not sure if
          // file.fail() implies that it's still open, but I'll
          // close it just in case
          tst_file.close();
        }
      }
  }
  // if paths.empty(), return ""
  // otherwise, return whatever was produced in the first else branch
  return ret_str;
}


// Ridiculously complicated string split function, since STL for some reason
//  doesn't have one. I tested this on several strings with different delimiters
//  in different places, it works as expected.
// Returns:
//  - A vector containing the words originally contained in the first parameter,
//    split by the delimiter in the second parameter.
//  - An empty vector if delimiter is absent in the first parameter.
std::vector<std::string> split(const std::string& tobesplit, const std::string delimiter) {
  std::vector<std::string> splitted_words{};
  std::string tbs2(tobesplit);
  size_t beg{0};
  size_t end{tbs2.find(delimiter)};
  if (tbs2.find(delimiter) == std::string::npos) {
    // string does not contain delimiter; return empty vector
    //return splitted_words;
  } else {
      if (tbs2.find(delimiter) == 0) {
        beg = delimiter.size();
        end = tbs2.find(delimiter,beg); //might be std::string::npos
      }

      do {
        std::string tmp{tbs2.substr(beg,end-beg)};
        splitted_words.push_back(tmp);
        if (splitted_words.back().empty()) {
          splitted_words.pop_back();
        }
        beg = end + delimiter.size();
        end = tbs2.find(delimiter,beg);
      } while(tbs2.find(delimiter,beg) != std::string::npos);
      // add last substring
      std::string tmp{tbs2.substr(beg,end-beg)};
      splitted_words.push_back(tmp);
      // remove trailing empty string which sometimes happens
      if(splitted_words.back().empty()){
        splitted_words.pop_back();
      }
  }
  // splitted_words will be empty if the first if statement failed;
  // otherwise it will be whatever happened in the else branch
  return splitted_words;
}
