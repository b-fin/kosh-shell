// Interface file for the second version of the AST structure.

#ifndef __AST_H
#define __AST_H

#include <iostream>
#include <string>
#include <vector>

class Arguments {
private:
  std::vector<std::string> m_arguments;
  int m_arg_count;
public:
  Arguments(char *in_str)
    : m_arguments{}, m_arg_count{0} {
      std::string arg1(in_str);
      m_arguments.push_back(arg1);
      m_arg_count++;
      free(in_str);
    };
  ~Arguments()=default;
  void add_argument(std::string);
  void print() const;
};


class Set {
private:
  std::string m_varname;
  std::string m_value;

public:
  Set(char *in1, char *in2)
    : m_varname{}, m_value{} {
      m_varname.assign(in1);
      m_value.assign(in2);
      free(in1);
      free(in2);
    };
  ~Set()=default;
  void print() const;
};

class S_command {
private:
  bool m_is_set = false;
  Set *m_set;
  std::string m_cmd_word;
  Arguments *m_arguments;
  int m_arg_count;

public:
  S_command(Set *in_set, char *in_cw, Arguments *in_arg)
    : m_set(in_set), m_cmd_word{}, m_arguments(in_arg) {
      m_cmd_word.assign(in_cw);
      free(in_cw);
    };
  ~S_command();
  //void add_arguments(const Arguments*);
  void print() const;
  void make_set();
};


class For {
  // stub
};

class C_command {
  // stub
};

class Command {
private:
  // one of these must be nullptr
  S_command *m_s_command;
  C_command *m_c_command;
public:
  Command(S_command* in_s, C_command* in_c)
    : m_s_command(in_s), m_c_command(in_c) {};
  ~Command();
  void print() const;

};

class List {
  //stub
};

class Redirect {
private:
  std::string m_type;
  std::string m_filename;
public:
  Redirect(char *in1, char *in2)
    : m_type{}, m_filename{} {
      m_type.assign(in1);
      m_filename.assign(in2);
      free(in1);
      free(in2);
    };
  ~Redirect() = default;
  void print() const;
};

class Pipe_seq {
private:
  Pipe_seq *m_pipe_seq;
  Command *m_command;
public:
  Pipe_seq(Pipe_seq *in_p, Command *in_c)
    : m_pipe_seq(in_p), m_command(in_c) {};
  ~Pipe_seq();
  void  print() const;
};

class Ccs {
private:
  // Eventually, we will want to keep track of:
  //   - how many redirects, of what kind, and where
  //   - how many pipes and where
  Ccs *m_ccs;
  Pipe_seq *m_pipe_seq;
  Redirect *m_redirect;
public:
  Ccs(Ccs *in_cc, Pipe_seq *in_p, Redirect *in_r)
    : m_ccs(in_cc), m_pipe_seq(in_p), m_redirect(in_r) {};
  ~Ccs();
  void print() const;
};

class Program {
private:
  // Eventually, we will want to keep track of:
  //    - how many complete commands (separated by ';')
  //    - background execution?
  Ccs *m_ccs;
  bool m_background;
public:
  Program(Ccs *in_cc)
    : m_ccs(in_cc) {};
  ~Program();
  void print() const;
  void set_bg();
};





#endif
