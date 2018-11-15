/* Grammar modified from POSIX shell specification */
/* NEW BISON FILE 11/16/18 */
%{
#include <iostream>
#include <string>
extern void yyerror(std::string);
extern int yylex();
extern int yyparse();
int option_count = 0;
%}
/* no arguments after redirect */
%union {
  char *str;
}
%token <str> WORD NAME FOR DGREAT SET STRING FROM
%token NEWLINE WHITESPACE

%nonassoc WORD2
%nonassoc WORD

%start program
%%
program     : command /*NEWLINE*/ { option_count=0; }
            /*| NEWLINE */
            ;
command     : cmd_word options
            | cmd_word
            ;
options     : options WORD
              {
                option_count++;
                std::cout << "Option # " << option_count
                << ": " << $2 << std::endl;
              }
            | WORD {
                option_count++;
                std::cout << "Option # " << option_count
                << ": " << $1 <<  std::endl;
                    }
            ;
cmd_word    : WORD { std::cout << "Found a word: " << $1 << std::endl; }
            ;
