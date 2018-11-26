# KOSH Shell

## Goal
>The goal of this project is to implement a small but powerful shell program that works like many of the standard unix shells: bash, csh, etc. Your learning objective is to write this shell (kosh) while using many of the principles you’ve learned in the operating systems and compilers classes.

-From the assignment page.

## Requirements
### Basic Functionality:
* 5 built-in commands:
    * `cd`
    * `pwd`
    * `source`
    * `exit`
    * `echo`
* Variables:
    * Users should be able to set their own variables with `set varname = value` (see Design Choices below)
* Executing external commands and executables, eg `./shell`, `bin/ls`. 
* Informative error messages and failure conditions. 
### Advanced Functionality:
* PATH variable 
	* Shell should be able to search this variable for all valid paths to a program.
* Background execution with `&`
* File redirection:
	* All four of the following redirections should be supported:
		* `<`, `>`, `>>`, `|`
* Support for `;` operator.
* Support for `$()` operator.
* Support for `\` character to escape metacharacters. 
* Support for wildcard operators `*` and `?`. 
* Support for looping over lists:
	* ```for varname from list: statements```

## Development notes: 
As of *November 25, 2018*, I have the following modules in working order or in progress:
* AST interface/implementation:
	* This is going to be the structure that Bison will build up and evaluate during its parse.
	* I have a class each for Arguments, for Commands, and for Programs (working with simplified grammar).
* shell.y:
	* This is the Bison grammar file; I am currently working with a simplified version of the grammar as I get the AST and various modules in working order. As I build up the modules, the grammar will be fleshed out to handle the full functionality.
* shell.l:
	* This is the Flex token file; it's pretty barebones but it works. 
* main.c:
	* This is a simple main file that sets up the `YY_BUFFER_STATE`	to point at stdin, then takes in a string, and then parses it using Bison (which also evaluates it), repeating this is in a loop until `exit` is received.
* Shell interface/implementation:
	* Barebones for now; primary method is run(), which sets up the call to yyparse() and calls Program_node->eval().

As of November 25, 2018:  
4 of the 5 built-in commands are functional, namely `exit`, `echo`, `pwd`, and `cd`. `source` is going to be a tough one and I'll be spending some time figuring that one out. After that, I'll be working on getting variables working. 
