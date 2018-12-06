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
%x SET_NAME
%x SET_EQUAL
%x SET_LAST_WORD
%%
[ \t\n]+
 /*"for"                             { yylval.str = strdup(yytext); BEGIN FOR_SET; return FOR; }*/
<INITIAL>"set"                       { yylval.str = strdup(yytext); BEGIN SET_NAME; return SET; }
<FOR_SET>"from"                      { yylval.str = strdup(yytext); return FROM; }
">>"                                 { yylval.str = strdup(yytext); return DGREAT; }
";"                                  { yylval.str = strdup(yytext); return SEMICOLON; }
"|"                                  { yylval.str = strdup(yytext); return PIPE; }
":"                                  { yylval.str = strdup(yytext); return COLON; }
"<"                                  { yylval.str = strdup(yytext); return LESS; }
">"                                  { yylval.str = strdup(yytext); return GREAT; }
<INITIAL>"="                         { yylval.str = strdup(yytext); return EQUAL; }
"&"                                  { yylval.str = strdup(yytext); return AMP; }
<INITIAL>\"[^\t\n]+\"                { yylval.str = strdup(yytext); return STRING; /* anything enclosed in quotes */}
[^ \t\n]+                            { yylval.str = strdup(yytext); return WORD; }
<SET_EQUAL,SET_NAME,SET_LAST_WORD>[ \t\n]
<SET_EQUAL>"="                        {yylval.str = strdup(yytext); BEGIN SET_LAST_WORD; return EQUAL; }
<SET_LAST_WORD>\"[a-zA-Z 0-9]+\"      { yylval.str = strdup(yytext); BEGIN INITIAL; return SETSTRING; }
<SET_LAST_WORD>[^ \t\n]+              { yylval.str = strdup(yytext); BEGIN INITIAL; return WORD; }
<SET_NAME>[a-zA-Z\_][a-zA-Z\_0-9]+    { yylval.str = strdup(yytext); BEGIN SET_EQUAL; return NAME; /* for varnames in FOR and SET */ }
.                                     { ; }
%%

void yyerror(Program& r, std::string s){
  std::cout << "Error detected: " << s  << ": " << "'" << yytext << "'" << std::endl;
  BEGIN INITIAL;
  // nonsense function to keep compiler happy re: bison parameter
  r.do_nothing();
}
