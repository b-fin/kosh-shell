# KOSH Shell

## Goal
>The goal of this project is to implement a small but powerful shell program that works like many of the standard unix shells: bash, csh, etc. Your learning objective is to write this shell (kosh) while using many of the principles you’ve learned in the operating systems and compilers classes.

-From the assignment page.
### Basic Functionality:
* 5 built-in commands: **done**
    * `cd`
    * `pwd`
    * `source`
    * `exit`
    * `echo`
* Variables: **done**
    * Users should be able to set their own variables with `set varname = value` (see Design Choices below)
* Executing external commands and executables, eg `./shell`, `bin/ls`.  **done**
* Informative error messages and failure conditions. 
### Advanced Functionality:
* PATH variable **done**
	* Shell should be able to search this variable for all valid paths to a program.
* Background execution with `&` **almost**
* File redirection: **not done**
	* All four of the following redirections should be supported:
		* `<`, `>`, `>>`, `|`
* Support for `;` operator. **almost**
* Support for `$()` operator. **not done**
* Support for `\` character to escape metacharacters. **almost**
* Support for wildcard operators `*` and `?`.  **not done**
* Support for looping over lists: **not done**
	* ```for varname from list: statements```
## Overview / Features


## Instructions for building / running KOSH
### Required software
KOSH was designed using the following software; it may build and run using different versions, but this is not explicitly supported.
* flex 2.6.4 or later 
* bison 3.2 or later 
* GNU make 4.1 or later
* GCC 7 or later 
* C++14
### Building
To build KOSH, clone the repository, open a terminal and navigate to the directory, and run:  
`make`.  
If you get an error related to `shell.tab.h`, run the following before `make`:  
`make flexbison`.  
Once it compiles, simply run the `shell` executable with:  
`./shell`.  
To clean the directory of object files, run:  
`make clean`
### Running   
Once KOSH is running, you may type in commands as you would any other shell application (pursuant to KOSH's grammar and syntax). Try:  `echo hello`.  

To exit KOSH, type:  
`exit`.

## Source code overview 
The code is largely contained in the following files:  
* Shell.h / Shell.cpp 
* AST.h / AST.cpp
* main.cpp
* shell.l
* shell.y
* var_expansions.h / var_expansions.cpp
#### Shell.h / Shell.cpp
These contain the declaration and definition, respectively, of the Shell class. The Shell class has an `std::unordered_map` of `std::string`s that represents the symbol table used to keep track of variables set by the user. Shell's important methods are:  
 * `int Shell::run()` which sets up the data for `yyparse()`, executes the resulting AST, and returns the exit status of whatever was executed.
 * `int Shell::execute(Program&)` which extracts information from the parameter about what execution to perform: built-in or external. It then calls and returns the exit status of the respective methods. 
 * `int Shell::execute_built_in(Program&)` which calls and returns the exit status of the respective built-in command.
 * `int Shelll::execute_external(Program&)` which calls and returns the exit status of an external command.
#### AST.h / AST.cpp
These contain the declaration and definition, respectively, of the AST class, which I like to visualize as such:  
![alt text][AST]

[AST]: https://github.com/b-fin/kosh-shell/blob/master/AST.png "AST"

## Design choices

## Design timeline 

## Limitations / Known errors

## Personal notes

## References 

## Development notes: 
As of *November 19, 2018*, I have the following modules in working order or in progress:
* AST interface/implementation:
	* This is going to be the structure that Bison will build up and evaluate during its parse.
	* I have a class each for Arguments, for Commands, and for Programs (working with simplified grammar).
* shell.y:
	* This is the Bison grammar file; I am currently working with a simplified version of the grammar as I get the AST and various modules in working order. As I build up the modules, the grammar will be fleshed out to handle the full functionality.
* shell.l:
	* This is the Flex token file; it's pretty barebones but it works. 
* main.c:
	* This is a simple main file that sets up the `YY_BUFFER_STATE`	to point at stdin, then takes in a string, and then parses it using Bison (which also evaluates it), repeating this is in a loop until `exit` is received.
