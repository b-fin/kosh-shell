%{
//#include "lex.yy.h"
#include <iostream>
#include <string>
extern void yyerror(std::string);
extern int yylex();
extern int yyparse();
%}

%union {
  char *str;
  int num;
}
%token <str> VALUE_STRING
%token <str> VARNAME
%token <str> LS
  <str> BAD_TOKEN
  <str> PIPE
  <str> PWD
  <str> FOR
  <str> SET
  <str> FROM
  <str> EXIT
  <str> CD
  <str> SOURCE
  <str> IDENTIFIER
  <str> APPENDTO
  <str> REDIRECTTO
  <str> REDIRECTFROM
  <str> FILENAME
  <str> SEMICOLON
  <str> DASH

%%

command_seq:
  command_seq op_control_seq { std::cout << "command_seq op_control command " << std::endl; }
  | command_seq op_redirect_seq { std::cout << "Bison found a command " << std::endl; }
  | command { std::cout << "command (under command_seq) " << std::endl; }
;
command:
  set_seq { std::cout << "set sequence" << std::endl; }
  //SET VARNAME '=' VALUE_STRING { std::cout << "set varname: " << $2 << " = value_string: " << $4 << std::endl; }
  | for_seq { std::cout << "for sequence" << std::endl; }
  //| FOR VARNAME FROM ':' command { std::cout << "for varname: " << $2 << " from list : command " << std::endl; }
  | LS arglist { std::cout << "ls arglist" << std::endl; }
  | LS { std::cout << "ls" << std::endl;}
  | PWD arglist { std::cout << "pwd arglist" << std::endl; }
  | PWD { std::cout << "pwd " << std::endl; }
  | EXIT arglist { std::cout << "exit arglist" << std::endl; }
  | EXIT { std::cout << "exit " << std::endl; }
  | CD arglist { std::cout << "cd arglist" << std::endl; }
  | CD { std::cout << "cd " << std::endl; }
  | SOURCE arglist { std::cout << "source arglist" << std::endl; }
  | SOURCE  { std::cout << "source " << std::endl; }
  | IDENTIFIER arglist '&'{ std::cout << "identifier: " << $1 << " arglist &(under command)" << std::endl; }
  | IDENTIFIER arglist { std::cout << "identifier: " << $1 << " arglist (under command)" << std::endl;}
  | IDENTIFIER { std::cout << "identifier: " << $1 << " (under command)" << std::endl; }
  | '.' '/' IDENTIFIER arglist '&'{ std::cout << "./ identifier: " << $3 << " arglist &" << std::endl; }
  | '.' '/' IDENTIFIER arglist { std::cout << "./ identifier: " << $3 << " arglist" << std::endl; }
  | '.' '/' IDENTIFIER  { std::cout << "./ identifier: " << $3 << " " << std::endl; }
;
set_seq:
  SET VARNAME '=' VALUE_STRING { std::cout << "SET VARNAME = VALUE_STRING (under set_seq) " << std::endl; }
  ;
for_seq:
  FOR VARNAME FROM ':' command { std::cout << "FOR VARNAME FROM : command (under for_seq)" << std::endl; }
  ;
op_redirect_seq:
  APPENDTO FILENAME { std::cout << "append to filename: " << $2 << std::endl; }
  | REDIRECTTO FILENAME { std::cout << "redirect to filename: " << $2 << std::endl; }
  | REDIRECTFROM FILENAME { std::cout << "redirect from filename: " << $2 << std::endl; }
  ;
op_control_seq:
  SEMICOLON command { std::cout << "SEMICOLON command (under op_control_seq)" << std::endl; }
  | PIPE command { std::cout << "PIPE command (under op_control_seq)" << std::endl; }
  ;
arglist:
  argument { std::cout << "argument (under arglist) " << std::endl; }
  | arglist argument { std::cout << "arglist argument (under arglist) " << std::endl; }
;

argument:
  IDENTIFIER { std::cout << "identifier: " << $1 << " (under argument)" << std::endl; }
  | DASH IDENTIFIER { std::cout << "-identifier: " << $2 << " (under argument)" << std::endl; }
  | FILENAME { std::cout << "filename: " << $1 << " (under argument) " << std::endl; }
;
%%
