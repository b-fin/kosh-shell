/* Grammar modified from POSIX shell specification */
/* NEW BISON FILE 11/16/18 */
%code requires {
#include "AST.h"
}

%{
#include <iostream> // std::cout<<
#include <string>

extern void yyerror(std::string);
extern int yylex();
extern int yyparse();
int option_count = 0;
%}
/* no arguments after redirect */
%union {
  char *str;
  //std::string str;
  Argument_node *arg_node;
  Command_node *cmd_node;
  Program_node *prgm_node;
}
%type <cmd_node> command
%type <prgm_node> program
%type <arg_node> options
%type <str> cmd_word
%token <str> WORD NAME FOR DGREAT SET STRING FROM
%token NEWLINE WHITESPACE

%nonassoc WORD2
%nonassoc WORD

%start program
%%
program     : command options
              {
                $$ = new Program_node($1);
                std::cout << "allocated(?) Program_node" << std::endl;
                $$->print();
                $$->eval();
                delete $$;
                std::cout << "freed(?) Program_node" << std::endl;
                option_count=0;
              }
            | command
              {
                $$ = new Program_node($1);
                $$->print();
                $$->eval();
                delete $$;
                option_count=0;
              }
            ;
command     : cmd_word
              { $$ = new Command_node($1);
                std::cout<< "Found cmd_wrd (under command)" << std::endl;
              }
            ;
options     : options WORD
              { $$ = new Argument_node($2);
                $<cmd_node>0->add_argument($$);
                option_count++;
                std::cout << "(p1)Option # " << option_count
                << ": " << $2 << std::endl;
              }
            | WORD
              { $$ = new Argument_node($1);
                $<cmd_node>0->add_argument($$);
                option_count++;
                std::cout << "(p2)Option # " << option_count
                << ": " << $1 <<  std::endl;
              }
            ;
cmd_word    : WORD { $$ = $1; std::cout << "Found a word: " << $1 << std::endl; }
            ;
%%
