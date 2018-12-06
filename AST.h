// Interface file for the second version of the AST structure.

#ifndef __AST_H
#define __AST_H

#include <iostream>
#include <string>
#include <vector>
#include "var_expansion.h"
#include <unordered_map>



class Arguments {
public:
  // Data members
  std::vector<std::string> m_arguments;
  int m_arg_count;

  // Methods
  Arguments(char *in_str)
    : m_arguments{}, m_arg_count{0} {
      std::string arg1(in_str);
      m_arguments.push_back(arg1);
      m_arg_count++;
      free(in_str);
    };
  Arguments(const Arguments &source)
    : m_arguments{}, m_arg_count{0} {
      for (int i=0; i<source.m_arg_count; i++) {
        std::string new_arg(source.m_arguments[i]);
        m_arguments.push_back(new_arg);
        m_arg_count++;
      }
    };
  Arguments& operator=(const Arguments &source);
  ~Arguments()=default;
  void add_argument(char *);
  void print() const;
  int get_arg_count() const;
  // Unused function for now
  void remove_quotes()=delete;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;

};


class Set {
public:
  // Data members
  std::string m_varname;
  std::string m_value;
  // Methods
  Set(char *in1, char *in2)
    : m_varname{}, m_value{} {
      m_varname.assign(in1);
      m_value.assign(in2);
      free(in1);
      free(in2);
    };
  Set(const Set &source)
    : m_varname{source.m_varname}, m_value{source.m_value} {};
  Set& operator=(const Set &source);
  ~Set()=default;
  // Unused function for now
  void remove_quotes()=delete;
  void print() const;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;

};

class S_command {
public:
  // Data members
  bool m_is_set = false;
  Set *m_set;
  std::string m_cmd_word;
  Arguments *m_arguments;

  // Methods
  S_command(Set *in_set, char *in_cw, Arguments *in_arg)
    : m_set(in_set), m_cmd_word{}, m_arguments(in_arg) {
      if (!in_cw) {
        m_cmd_word = "set";
        m_is_set = true;
      } else {
        m_cmd_word.assign(in_cw);
        free(in_cw);
      }
    };
  S_command(const S_command &source)
    : m_set{nullptr}, m_cmd_word{source.m_cmd_word}, m_arguments{nullptr} {
      if (source.m_set) {
        m_set = new Set(*source.m_set);
        m_is_set = true;
      }
      else if (source.m_arguments) {
        m_arguments = new Arguments(*source.m_arguments);
      }
    };
  S_command& operator=(const S_command &source);
  ~S_command();
  void print() const;
  bool has_args() const { bool ret; (m_arguments) ? ret=true : ret=false; return ret; } 
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;
  bool cmd_word_needs_expanding() const;
  bool args_need_expanding() const;

};


class For {
  // stub
};

class C_command {
  // stub (planned to support `for` loops)
};

class Command {
public:
  // Date Members
  // Note: one of these must be nullptr (cannot have both at once)
  S_command *m_s_command;
  C_command *m_c_command;
  // Methods
  Command(S_command* in_s, C_command* in_c)
    : m_s_command(in_s), m_c_command(in_c) {};
  Command(const Command& source)
    : m_s_command{nullptr}, m_c_command{nullptr} {
      //std::cout<< "COMMAND copy ctor called\n";
      if (source.m_s_command) { m_s_command = new S_command(*source.m_s_command); }
      else if (source.m_c_command) { m_c_command = new C_command(*source.m_c_command); }
    };
  Command& operator=(const Command& source);
  ~Command();
  void print() const;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;


};

class List {
  //stub (also for `for` loops)
};

class Redirect {
public:
  // Date members
  std::string m_type;
  std::string m_filename;
  // Methods
  Redirect(char *in1, char *in2)
    : m_type{}, m_filename{} {
      m_type.assign(in1);
      m_filename.assign(in2);
      free(in1);
      free(in2);
    };
  Redirect(const Redirect& source)
    : m_type{source.m_type}, m_filename{source.m_filename} {};
  Redirect& operator=(const Redirect& source);
  ~Redirect() = default;
  void print() const;
};

class Pipe_seq {
public:
  // Data members
  Pipe_seq *m_pipe_seq;
  Command *m_command;
  // Methods
  Pipe_seq(Pipe_seq *in_p, Command *in_c)
    : m_pipe_seq(in_p), m_command(in_c) {};
  Pipe_seq(const Pipe_seq& source)
    : m_pipe_seq{nullptr}, m_command{nullptr} {
      if (source.m_pipe_seq) {
        m_pipe_seq = new Pipe_seq(*source.m_pipe_seq);
      }
      if (source.m_command) {
        m_command = new Command(*source.m_command);
      }
    };
  Pipe_seq& operator=(const Pipe_seq& source);
  // for use in shell.y
  void add_pipe_seq(Pipe_seq *source) { m_pipe_seq = source; }
  ~Pipe_seq();
  void  print() const;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;

};

class Ccs {
public:
  // Date members
  // Eventually, we will want to keep track of:
  //   - how many redirects, of what kind, and where
  //   - how many pipes and where
  Ccs *m_ccs;
  Pipe_seq *m_pipe_seq;
  Redirect *m_redirect;
  // Methods
  Ccs(Ccs *in_cc, Pipe_seq *in_p, Redirect *in_r)
    : m_ccs(in_cc), m_pipe_seq(in_p), m_redirect(in_r) {};
  Ccs(const Ccs& source)
    : m_ccs{nullptr}, m_pipe_seq{nullptr}, m_redirect{nullptr} {
      if (source.m_ccs) {
        m_ccs = new Ccs(*source.m_ccs);
      }
      if (source.m_pipe_seq) {
        m_pipe_seq = new Pipe_seq(*source.m_pipe_seq);
      }
      if (source.m_redirect) {
        m_redirect = new Redirect(*source.m_redirect);
      }
    };
  Ccs& operator=(const Ccs& source);
  // for use in shell.y
  void add_ccs(Ccs *source) { m_ccs = source; };
  ~Ccs();
  void print() const;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;

};

class Program {
public:
  // Data members
  Ccs *m_ccs;
  bool m_background=false;
  // Methods
  Program(Ccs *in_cc)
    : m_ccs(in_cc) {};
  Program()
    : m_ccs(nullptr), m_background(false) {};
  Program(const Program& source)
    : m_ccs{nullptr} {
      if (source.m_ccs) {
        m_ccs = new Ccs(*source.m_ccs);
      }
      if (source.m_background) {
        m_background = true;
      }
    };
  Program& operator=(const Program& source);
  ~Program();
  void print() const;
  // Useless function for shell.l
  void do_nothing() const;
  bool expand_vars(const std::unordered_map<std::string,std::string>& in_s_t);
  bool needs_expanding() const;
};





#endif
