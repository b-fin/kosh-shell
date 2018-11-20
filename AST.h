// The interface file for our AST class. For now it will mostly
//  handle simple command nodes, later we'll beef it up to handle
//  semicolon operators, pipe operators, and redirection operators.

// Begin header guards
#ifndef _AST_H
#define _AST_H
// End header guards

// Begin includes:
#include <vector> //for holding arguments in Command_node?
#include <iostream>
// End includes
/*
class AST_node {
private:

public:
  AST_node() = default;
  virtual ~AST_node() = 0;
};
*/
class Argument_node /*: public AST_node */{
private:
  char *m_argument;
  Argument_node* m_next; // !!!!!! remember to account for this 
public:
  Argument_node(char* in_arg)
    : m_argument(in_arg){};
  ~Argument_node() = default;
  void print();
};



class Command_node /*: public AST_node */{
private:
  // I'm wondering if command will have any children later in the project.
  //    My inclination is no.
  int m_argument_count;
  std::vector<Argument_node*> m_arguments;
  char* m_cmd_word;
public:
  // On initialization, the vector will have size 0,
  //  arg_count is 0, and word is whatever word flex returnhttps://app.e2ma.net/app2/audience/signup/1826152/1722524/?r=signups
  Command_node(char* cmd_input)
    : m_argument_count(0),m_arguments(0), m_cmd_word(cmd_input) {};
  ~Command_node();
  // what operations do we want for a command_node?
  // set_command_word? update: not necessary
  // add_argument?
  void add_argument(Argument_node*);
  void print();
};

// Might be used for holding command nodes
class Program_node /*: public AST_node*/ {
private:
  Command_node* m_command;
  // later will have room for other shits
public:
  Program_node(Command_node* cmd_node_inpt)
    : m_command(cmd_node_inpt) {};
  ~Program_node();
  // What other operations? eval?
  void print();
};

#endif // _AST_H
