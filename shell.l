%option noyywrap
%option header-file="lex.yy.h"
%{
#include "shell.tab.h"
#include <iostream>
#include <string>
#include <string.h>
/* Known issues 11/12:
 * - in arguments, flex mistakes filenames (eg apples.txt) as identifiers.
 *    - => may have fixed that by switching the precedence of ids and fns.
 * - in set and for sequences, flex flags the first varname as an error.
 *    - => not recognizing it as a VARNAME?
 */

%}
%%
[ \t\n]+ ;
"set" { yylval.str = strdup(yytext); return SET; }
"cd" { yylval.str = strdup(yytext); return CD; }
"exit" { yylval.str = strdup(yytext); return EXIT; }
"source" { yylval.str = strdup(yytext); return SOURCE; }
"pwd" { yylval.str = strdup(yytext); return PWD; }
"ls" { yylval.str = strdup(yytext); return LS; }
"for" { yylval.str = strdup(yytext); return FOR; }
"from" { yylval.str = strdup(yytext); return FROM; }
">" { yylval.str = strdup(yytext); return REDIRECTTO; }
">>" { yylval.str = strdup(yytext); return APPENDTO; }
"|" { yylval.str = strdup(yytext); return PIPE; }
"<" { yylval.str = strdup(yytext); return REDIRECTFROM; }
";" { yylval.str = strdup(yytext); return SEMICOLON; }
"-" { yylval.str = strdup(yytext); return DASH; }
[^ \t\n]+\.([a-zA-Z]{1,6}) { yylval.str = strdup(yytext); return FILENAME; }
[a-zA-Z][a-zA-Z\_\.\+\-]* { yylval.str = strdup(yytext); return IDENTIFIER /* for commands */; }
[a-zA-Z][a-zA-Z\_0-9]+ { yylval.str = strdup(yytext); return VARNAME; /* for varnames in FOR and SET */ }
\"[a-zA-Z 0-9]+\" { yylval.str = strdup(yytext); return VALUE_STRING; /* for strings in SET statements */ }
. { return BAD_TOKEN; }

%%

void yyerror(std::string s){
  std::cout << "ah! an error! " << s  << ": " << "'" << yytext << "'" << std::endl;

  }
