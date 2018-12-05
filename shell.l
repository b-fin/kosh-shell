%option noyywrap
%option header-file="lex.yy.h"
%top{
#include "shell.tab.h"
#include "AST.h"
#include <iostream>
#include <string>
#include <string.h>
/* NEW LEXER FILE 11/14/18 */
}
%x FOR_SET
%%
[ \t]+ /* eat tabs and spaces (but not newlines) */;
\n { return NEWLINE; }
"for" { yylval.str = strdup(yytext); BEGIN FOR_SET; return FOR; }
"set" { yylval.str = strdup(yytext); BEGIN FOR_SET; return SET; }
<FOR_SET>"from" { yylval.str = strdup(yytext); return FROM; }
">>" { yylval.str = strdup(yytext); return DGREAT; }
";" { yylval.str = strdup(yytext); return SEMICOLON; }
"|" { yylval.str = strdup(yytext); return PIPE; }
":" { yylval.str = strdup(yytext); return COLON; }
"<" { yylval.str = strdup(yytext); return LESS; }
">" { yylval.str = strdup(yytext); return GREAT; }
"=" { yylval.str = strdup(yytext); return EQUAL; }
"&" { yylval.str = strdup(yytext); return AMP; }
[^ \t\n]+ { yylval.str = strdup(yytext); return WORD; }
<FOR_SET>[a-zA-Z\_][a-zA-Z\_0-9]+ { yylval.str = strdup(yytext); BEGIN INITIAL; return NAME; /* for varnames in FOR and SET */ }
\"[a-zA-Z 0-9]+\" { yylval.str = strdup(yytext); return STRING; /* for strings in SET statements */ }
%%

void yyerror(Program& r, std::string s){
  std::cout << "Error detected: " << s  << ": " << "'" << yytext << "'" << std::endl;
  r.print();

  }
