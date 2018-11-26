// The interface file for our AST class. For now it will mostly
//  handle simple command nodes, later we'll beef it up to handle
//  semicolon operators, pipe operators, and redirection operators.

// Begin header guards
#ifndef _AST_H
#define _AST_H

// Begin includes:
#include <vector>
#include <iostream>
#include <string>
// End includes


//      || Argument Node ||     //
class Argument_node {
private:
  std::string m_argument;
public:
  Argument_node(char* in_arg)
    : m_argument(in_arg){ free(in_arg); };
  ~Argument_node() = default;
  void print();
  std::string get_word();
};



//        || Command Node ||      //
class Command_node {
private:
  int m_argument_count;
  std::vector<Argument_node*> m_arguments;
  std::string m_cmd_word;
public:
  Command_node(char* cmd_input)
    : m_argument_count(0),m_arguments(0), m_cmd_word(cmd_input)
    { free(cmd_input); };
  ~Command_node();
  void print();
  void add_argument(Argument_node*);
  bool has_arguments();
  int get_arg_count();
  std::string get_word();
  std::string arguments_string();
  std::string get_argument_n(int n);
};


//      || Program Node ||      //
class Program_node {
private:
  Command_node* m_command;
public:
  Program_node(Command_node* cmd_node_inpt)
    : m_command(cmd_node_inpt) {};
  Program_node()
    : m_command(nullptr) {};
  ~Program_node();
  void add_command_node(Command_node*);
  void print();
  int call_cd();
  int call_exit();
  int call_echo();
  int call_source();
  int call_pwd();
  int eval();
  std::string& get_eval_string();
};


extern Program_node* prg_node;
#endif // End header guards
