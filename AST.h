// The interface file for our AST class. For now it will mostly
//  handle simple command nodes, later we'll beef it up to handle
//  semicolon operators, pipe operators, and redirection operators.

// Begin header guards
#ifndef _AST_H
#define _AST_H
// End header guards

// Begin includes:
#include <vector>
#include <iostream>
#include <string>
// End includes

//=============================================================================
//=============================================================================
//      || Argument Node ||     //
class Argument_node /*: public AST_node */{
private:
  std::string m_argument;
public:
  Argument_node(std::string in_arg)
    : m_argument(in_arg){};
  ~Argument_node() = default;
  void print();
  std::string get_word();
};

//=============================================================================
//=============================================================================
//        || Command Node ||      //
class Command_node /*: public AST_node */{
private:
  int m_argument_count;
  std::vector<Argument_node*> m_arguments;
  std::string m_cmd_word;
public:
  // On initialization, the vector will have size 0,
  //  arg_count is 0, and word is whatever word flex returnhttps://app.e2ma.net/app2/audience/signup/1826152/1722524/?r=signups
  Command_node(std::string cmd_input)
    : m_argument_count(0),m_arguments(0), m_cmd_word(cmd_input) {};
  ~Command_node();
  void add_argument(Argument_node*);
  void print();
  bool has_arguments();
  int get_arg_count();
  std::string get_word();
  std::string arguments_string();
};
//=============================================================================
//=============================================================================
//      || Program Node ||      //
class Program_node /*: public AST_node*/ {
private:
  Command_node* m_command;
  // later will have room for other shits
public:
  Program_node(Command_node* cmd_node_inpt)
    : m_command(cmd_node_inpt) {};
  ~Program_node();
  void print();
  int eval();
  std::string& get_eval_string();
};
//=============================================================================
//=============================================================================
#endif // _AST_H
