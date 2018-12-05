// Shell implementation. Primary functions are run() and execute() which will run
//  through the whole process of executing a command (expansions etc)
//  Secondary functions are the built-in executers.

#include "Shell.h"
#include <cassert>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <fstream>

#define KOSH_DEBUG_PRINT false


// Global variable, probably not going to be used
bool big_exit = 0;

// This is the big one; it contains the shell "loop" and sets up the input
// to be sent to yyparse(). Also takes care of deleting various allocated memories.
int Shell::run() {
  std::string in_string;
  //extern Program *root;
  Program *sh_root = new Program(nullptr);
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
  yyparse(*sh_root);
  Program *new_root = new Program(*sh_root);
  Program& prog_ref = *new_root;
  int exit_status = execute(prog_ref);
  yy_delete_buffer(bp);
  print_symbol_table();
  delete[] line_prefix;
  delete[] str_copy;
  delete sh_root;
  delete new_root;



  return exit_status;
}
// Other big function. Performs expansions and (eventually) redirections,
//  also makes calls to the various sub-execution methods eg execute_cd,
//  execute_internal.
int Shell::execute(Program& in_prog) {
  int exit_status = 0;

  bool expanded=true;
  if (in_prog.needs_expanding()) {
    expanded = expand_vars(in_prog);
  }
  std::cout<< "expanded is: " << expanded << std::endl;
  if(!expanded) {
    std::cout<< "EXECUTE: variable expansion failed\n";
    return -1;
  }

  Ccs *current;
  if (in_prog.m_ccs) {
    current = in_prog.m_ccs;
  } else {
    std::cout<< "EXECUTE: no m_ccs member\n";
    return -1;
  }


  // this while loop runs once during simple execution
  while (current != nullptr) {
    std::string cmd_to_be_checked(current->m_pipe_seq->m_command->m_s_command->m_cmd_word);
    if (is_built_in(*current->m_pipe_seq->m_command->m_s_command)) {
      exit_status = execute_built_in(*current->m_pipe_seq->m_command->m_s_command);
    } else {
      exit_status = execute_external(*current->m_pipe_seq->m_command->m_s_command);
    }
    // this will allow us to execute chains of commands
    current = current->m_ccs;
  }
  return exit_status;
}

bool Shell::is_built_in(S_command& in_com) {
  if (    in_com.m_cmd_word == "cd"  || in_com.m_cmd_word == "echo"
      || in_com.m_cmd_word == "pwd"  || in_com.m_cmd_word == "source"
      || in_com.m_cmd_word == "set"  || in_com.m_cmd_word == "exit"    )
      { return true; }
  // else, not built in
  return false;
}

bool Shell::_exit() {
  return shell_exit;
}

// Sorry this next function is a mess. It essentially checks if the
// input pointers are valid, checks if the cmd matches a built-in command,
// and executes accordingly. Returns -1 if an error occurs.
int Shell::execute_built_in(S_command &in_com) {
    if (in_com.m_cmd_word == "cd") {
      return execute_cd(in_com);
    } else if (in_com.m_cmd_word == "echo") {
      return execute_echo(in_com);
    } else if (in_com.m_cmd_word == "set") {
      return execute_set(in_com);
    } else if (in_com.m_cmd_word == "source") {
      return execute_source(in_com);
    } else if (in_com.m_cmd_word == "pwd") {
      return execute_pwd(in_com);
    } else if (in_com.m_cmd_word == "exit"){
      return execute_exit(in_com);
    } else {
        if (KOSH_DEBUG_PRINT) {
          std::cout<< "ERROR: something went wrong in execute_built_in()\n";
        }
      return -1;
    }
}

// This function is a mammoth, mostly because I couldn't find an elegant way
// to have the argv array built and returned from another function.
int Shell::execute_external(S_command &in_com) {
  // Prepare args
  int tmp_size = 2;
  bool args = in_com.has_args();

  if(args) {
    tmp_size = in_com.m_arguments->m_arg_count+2;
  }
  //char *argv[tmp_size];
  char **argv = new char*[tmp_size];

  if(args) {
    for(int i=1; i<=in_com.m_arguments->m_arg_count; i++) {
      argv[i] = strdup(in_com.m_arguments->m_arguments[i-1].c_str()); // free all these
    }
  }

  argv[0] = strdup(in_com.m_cmd_word.c_str()); // free this
  argv[tmp_size-1] = NULL;

  // Actually begin fork-exec sequence
  pid_t cpid, w;
  int wstatus;
  int sh_exit_status=0;
  cpid = fork();
  if(cpid == 0) {
    // we are in the child process
    if ( execvp(in_com.m_cmd_word.c_str(), const_cast<char *const(*)>(argv)) == -1) {
      std::cout<< "ERROR: error executing command (exec)\n";
      sh_exit_status=-1;
      exit(-1);
    }
  } else if (cpid < 0) {
    std::cout<< "ERROR: error executing command (fork)\n";
    sh_exit_status=-1;
  } else {
    // parent process
    do {
      w = waitpid(cpid,&wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
        std::cout<< "ERROR: error executing command (wait)\n";
        sh_exit_status = -1;
      }
    } while(!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }
  // FREE/DELETE
  for(int i=0; i<tmp_size; i++) {
    free(argv[i]);
  }
  delete[] argv;
  return sh_exit_status;
}

int Shell::execute_cd(const S_command& in_com){
  assert(in_com.m_cmd_word == "cd");
  int result{};
  if (!in_com.has_args()) {
    return chdir(getenv("HOME"));
  } else {
    // in_com has arguments
    if (in_com.m_arguments->m_arg_count != 1) {
      std::cout<< "ERROR: cd takes 1 argument, more given\n";
      return -1; // error, too many arguments
    } else {
      std::string cd_arg(in_com.m_arguments->m_arguments[0]);
      result = chdir(cd_arg.c_str());
      if (result != 0) {
        std::cout<< "ERROR: " << cd_arg << " not valid directory\n";
      }
      return result;
  }
  }
}

int Shell::execute_pwd(const S_command& in_com){
  assert(in_com.m_cmd_word == "pwd");
  char *temp = new char[512]; // long-ass char string to handle most cwd paths
  temp = getcwd(temp, 512);
  if (!temp) { return -1; } // error getting cwd
  std::cout<< temp << std::endl; // for REDIRECT, and PIPES, make sure to
                                // change std:cout to whatever stream is being redirected to
  delete[] temp;
  return 0;
}

int Shell::execute_source(const S_command& in_com){
  assert(in_com.m_cmd_word == "source");
  if (!in_com.has_args()) {
    std::cout << "ERROR: source expects 1 argument, 0 given\n";
    return -1;
  }
  if (in_com.m_arguments->m_arg_count != 1) {
    std::cout << "ERROR: source expects 1 argument, more given\n";
    return -1;
  }
  //  here we know that there is one argument, presumably a filename
  //  so, try to open it it (maybe search PATH for it)

  // ATTEMPT 1: search local directory
  std::ifstream infile;
  std::string filename {in_com.m_arguments->m_arguments[0]};
  std::string prefix;
  char *prefix_c = new char[256];
  getcwd(prefix_c,256);
  prefix.assign(prefix_c);
  std::string prefix_plus_filename = prefix + "/" + filename;
  //debug
  //std::cout<< "prefix_plus_filename: " << "\'"<< prefix_plus_filename <<"\'" <<std::endl;
  infile.open(prefix_plus_filename.c_str());
  if (infile.fail()) {
    infile.clear();
    infile.close();
    std::cout<< "Source: File not found in current dir, searching PATH\n";
    // ATTEMPT 2: search PATH
    if (search_path_for_file(filename).empty()) {
      std::cout<< "Source: File not found\n";
      delete[] prefix_c;
      return -1;
    }

    infile.open(search_path_for_file(filename).c_str());
    if (infile.fail()) {
      infile.clear();
      infile.close();
      std::cout<< "Source: Error opening file\n";
      delete[] prefix_c;
      return -1;
    }
  }
  // Okay here, we have the file open and ready for us. Let's pull out the lines and
  //  execute them one by one.

  std::string line;
  int source_exit_status;
  int count = 0;
  while (getline(infile,line)) {
    count++;
    // set up yyparse
    //extern Program *root;
    Program *src_root = new Program(nullptr);
    char *str_copy = new char[line.size()+1];
    strcpy(str_copy, line.c_str());
    YY_BUFFER_STATE bp = yy_scan_string(str_copy);
    yyparse(*src_root);
    Program *source_root = new Program(*src_root);
    Program& source_ref = *source_root;
    source_exit_status = execute(source_ref);
    if(source_exit_status != 0) {
      delete[] str_copy;
      yy_delete_buffer(bp);
      //delete root;
      delete source_root;
      std::cout<< "Source: Error executing commands\n";
      return -1;
    }
    delete[] str_copy;
    yy_delete_buffer(bp);
    delete src_root;
    delete source_root;
  }
  infile.clear();
  infile.close();
  // if we got here, thank god, source worked.
  if (KOSH_DEBUG_PRINT) {
    std::cout<< "thank god, we're out of source, count: "<<count<<"\n";
  }
  delete[] prefix_c;
  return 0;
}

int Shell::execute_exit(const S_command& in_com){
  assert(in_com.m_cmd_word == "exit");
  shell_exit = true;
  return 0;
}

int Shell::execute_echo(const S_command& in_com){
  assert(in_com.m_cmd_word == "echo");
  if (!in_com.has_args()) {
    // print empty line
    std::cout<<std::endl;
    return 0;
  }
  // else: we have an arguments node
  std::string output{};
  for (int i = 0; i < in_com.m_arguments->m_arg_count; i++) {
    output += in_com.m_arguments->m_arguments[i] + " ";
  }
  // remove trailing space
  output.pop_back();
  std::cout<< output << std::endl;
  return 0;
}

int Shell::execute_set(const S_command& in_com){
  assert(in_com.m_is_set);
  assert(in_com.m_cmd_word == "set");

  // check if varname is already in symbol table; if so, overwrite it
  if (m_symbol_table.count(in_com.m_set->m_varname) == 1) {
    m_symbol_table[in_com.m_set->m_varname] = in_com.m_set->m_value;
    return 0;
  }
  else {
    // insert into table as new element
    m_symbol_table.insert( {in_com.m_set->m_varname, in_com.m_set->m_value} );
    return 0;
  }
  // something went wrong
  std::cout<< "ERROR: Could not set variable\n";
  return -1;
}

void Shell::print_symbol_table() const {
  if (KOSH_DEBUG_PRINT) {
    std::cout<< "SYMBOL TABLE CONTENTS: " << std::endl;
    for (auto& x : m_symbol_table) {
      std::cout<< x.first<< ": "<< x.second << std::endl;
    }
  }
}


bool Shell::expand_vars(Program& in_prog) {
  int exit_status;
  exit_status = in_prog.expand_vars(m_symbol_table);
  return exit_status;
}

std::string Shell::search_path_for_file(std::string in_str) const {
  std::string total_path;
  total_path.assign(getenv("PATH"));
  std::vector<std::string> paths = split(total_path,":");
  if (paths.empty()) {
    // something went wrong
    return "";
  }
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
      //tst_file.close(); ????
      return prefix_plus_filename;
    }
    tst_file.close();
  }

  // no valid file found
  return "";
}


// Ridiculously complicated string split function, since STL for some reason
//  doesn't have one. I tested this on several strings with different delimiters
//  in different places, it works as expected.
std::vector<std::string> split(const std::string& tobesplit, const std::string delimiter) {
  std::vector<std::string> splitted_words{};
  std::string tbs2(tobesplit);
  size_t beg{0};
  size_t end{tbs2.find(delimiter)};
  if (tbs2.find(delimiter) == std::string::npos) {
    // string does not contain delimiter; return empty vector
    return splitted_words;
  }
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
  if(splitted_words.back().empty()){
    splitted_words.pop_back();
  }
  return splitted_words;

}
