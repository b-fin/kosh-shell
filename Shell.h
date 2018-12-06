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
#include "var_expansion.h"


class Shell {
private:
  std::unordered_map<std::string, std::string> m_symbol_table;
public:
  // Good idea from BK
  enum ExecStatus {
    // should never be returned
    EXEC_IN_PROG = -1,
    EXEC_SUCCESS = 0,
    EXEC_FAIL,
    // cmd_word does not match
    EXEC_ERR_CMD,
    // error regarding arguments
    EXEC_ERR_ARG,
    EXEC_ERR_VAR_EXP,
    EXEC_ERR_MISSING_NODE,
    EXEC_ERR_OTHER
  };

  bool shell_exit = false;
  Shell()
    : m_symbol_table{} {};
  // Default destructor until we have dynamically allocated data to delete
  ~Shell() = default;
  int run();
  int execute(Program& in_prog);
  bool is_built_in(S_command&);
  int execute_built_in(S_command& in_com);
  int execute_external(S_command& in_com);
  int execute_cd(const S_command&);
  int execute_pwd(const S_command&);
  int execute_source(const S_command&);
  int execute_exit(const S_command&);
  int execute_echo(const S_command&);
  int execute_set(const S_command&);
  void print_symbol_table() const;
  bool expand_vars(Program& in_prog);
  std::string search_path_for_file(std::string in_str) const;
};

std::vector<std::string> split(const std::string& tobesplit, const std::string delimiter);

#endif
