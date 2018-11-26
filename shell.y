/* Grammar modified from POSIX shell specification */
/* NEW BISON FILE 11/25/18 */
%code requires {
#include "AST.h"
}

%{
#include <iostream>
#include <string>
#include "Shell.h"

Program_node *root; /* Hopefully this will allow our AST to persist
                                * beyond the lifetime of yyparse() */
extern void yyerror(std::string);
extern int yylex();
extern int yyparse();
int option_count = 0;
%}

%union {
  char *str;
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


%start program
%%
program     : command options
              {
                $$ = new Program_node($1);
                root = $$;
                root->print();
                option_count=0;
              }
            | command
              {
                $$ = new Program_node($1);
                root = $$;
                root->print();
                option_count=0;
              }
            ;
command     : cmd_word
              { $$ = new Command_node($1); }
            ;
options     : options WORD
              { $$ = new Argument_node($2);
                $<cmd_node>0->add_argument($$);
                option_count++;
              }
            | WORD
              { $$ = new Argument_node($1);
                $<cmd_node>0->add_argument($$);
                option_count++;
              }
            ;
cmd_word    : WORD { $$ = $1; }
            ;
%%
