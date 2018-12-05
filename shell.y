/* nth attempt at a grammar */
/* 11/29/18 */
%code requires {
  #include "AST.h"
  #define YYDEBUG 1
  #include <iostream>
  #include <string>
  extern void yyerror(Program &, std::string);
  extern int yylex();
  //extern int yyparse(Program &);
  //int option_count = 0;
}
%define parse.trace true
%parse-param {Program& root}


%union {
  char *str;
  Program *prog;
  Pipe_seq *pipe_s;
  Ccs *complcom;
  Redirect *redir;
  List *lst;
  Command *cmd;
  S_command *scmd;
  C_command *ccmd;
  For *for_node;
  Set *set;
  Arguments *args;
}

%type <prog> program
%type <complcom> complete_commands
%type <pipe_s> pipe_sequence
%type <redir> redirect
%type <lst> list
%type <cmd> command
%type <scmd> simple_command
%type <ccmd> compound_command
%type <for_node> for_clause
%type <set> set_clause
%type <args> arguments
%type <str> cmd_word

%token <str> WORD NAME FOR DGREAT SET STRING FROM NEWLINE PIPE SEMICOLON
%token <str> COLON LESS GREAT EQUAL AMP SETSTRING

%{
/*
%destructor { free($$); } <str>
%destructor { delete($$); } <prog>
%destructor { delete($$); } <complcom>
%destructor { delete($$); } <pipe_s>
%destructor { delete($$); } <redir>
%destructor { delete($$); } <lst>
%destructor { delete($$); } <cmd>
%destructor { delete($$); } <scmd>
%destructor { delete($$); } <ccmd>
%destructor { delete($$); } <for_node>
%destructor { delete($$); } <set>
%destructor { delete($$); } <args>
*/
%}

%%

program           : complete_commands { $$ = new Program($1); root = *$$; delete $$;}
                  | complete_commands AMP { $$ = new Program($1); root = *$$; $$->set_bg(); free($2); delete $$; }
                  ;
complete_commands : complete_commands SEMICOLON pipe_sequence redirect
                        { Ccs *temp = new Ccs(nullptr,$3,$4); free($2);
                          $$ = $1; $$->add_ccs(temp); }
                  | complete_commands SEMICOLON pipe_sequence
                        { Ccs *temp = new Ccs(nullptr,$3,nullptr); free($2);
                          $$ = $1; $$->add_ccs(temp); }
                  | pipe_sequence redirect
                        { $$ = new Ccs(nullptr,$1,$2); }
                  | pipe_sequence
                        { $$ = new Ccs(nullptr,$1,nullptr); }
                  ;
pipe_sequence     : pipe_sequence PIPE command
                        { Pipe_seq *temp = new Pipe_seq(nullptr,$3); free($2);
                          $$ = $1; $$->add_pipe_seq(temp); }
                  | command { $$ = new Pipe_seq(nullptr,$1); }
                  ;
command           : simple_command { $$ = new Command($1,nullptr); }
                  | compound_command { $$ = new Command(nullptr,$1); }
                  ;
simple_command    : set_clause { $$ = new S_command($1,nullptr,nullptr); $$->make_set(); }
                  | cmd_word arguments { $$ = new S_command(nullptr,$1,$2); }
                  | cmd_word { $$ = new S_command(nullptr,$1,nullptr); }
                  ;
compound_command  : for_clause { /* stub */ }
                  ;
set_clause        : SET NAME EQUAL WORD { $$ = new Set($2,$4); free($1); free($3); }
                  | SET NAME EQUAL SETSTRING { $$ = new Set($2,$4); free($1); free($3); }
                  ;
for_clause        : FOR NAME FROM list COLON simple_command { free($1); free($3); }
                  ;
arguments         : arguments WORD { $$ = $1; $$->add_argument($2); }
                  | WORD { $$ = new Arguments($1); }
                  | STRING { $$ = new Arguments($1); }
                  ;
redirect          : LESS WORD { $$ = new Redirect($1,$2); }
                  | GREAT WORD { $$ = new Redirect($1,$2); }
                  | DGREAT WORD { $$ = new Redirect($1,$2); }
                  ;
cmd_word          : WORD { $$ = $1; }
                  | STRING { $$ = $1; }
                  ;
list              : list WORD { /* stub */ }
                  | WORD { /* stub */ }
                  ;
