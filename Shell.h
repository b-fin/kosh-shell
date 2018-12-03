// Shell interface; it will be a pretty abstract class, for the most part
//  it will keep track of variables and various flags related to the command
//  to be executed (eg has_unescaped_chars()) etc.

#ifndef __SHELL_H
#define __SHELL_H

#include <vector>
#include <iostream>
#include "AST.h"
#include <unordered_map>
#include "shell.tab.h"
#include "lex.yy.h"


class Shell {
private:
  std::unordered_map<std::string, std::string> m_symbol_table;
  // whatever else we'll need during the running of the program
public:
  bool shell_exit = false;
  Shell()
    : m_symbol_table{} {};
  // Change destructor if we have dynamically allocated members later
  ~Shell() = default;
  int run();
  int execute(Program *in_prog);
  bool is_built_in(S_command *);
  bool _exit();
  int execute_built_in(S_command *in_com, Arguments *in_arg);
  int execute_external(S_command *in_com, Arguments *in_arg);
  int execute_cd(const S_command*, const Arguments*);
  int execute_pwd(const S_command*);
  int execute_source(const S_command*, const Arguments*);
  int execute_exit(const S_command*);
  int execute_echo(const S_command*, const Arguments*);
  int execute_set(const S_command*);
};

#endif
