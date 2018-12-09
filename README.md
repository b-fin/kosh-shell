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
    * Users should be able to set their own variables with `set varname = value` 
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
##### Built-in Commands  
The commands that KOSH implements internally are:
 - `cd [directory]`: The shell changes the current directory to the `filename` argument, if valid; if no argument is given, it changes to the user's `$HOME`directory. If an invalid argument is supplied, no change is made.
 - `echo [arguments]`: The shell prints the optional arguments to the `stdin`; if no arguments are supplied, it prints a newline.
 - `pwd`: The shell prints the PWD to the `stdin`. 
 - `exit`: The shell exits with a status of 0 (normal exit). 
 - `source filename`: The shell attempts to locate the argument filename in the local directory; if it can't be located, it searches the `$PATH` variable; upon opening the file, it grabs the input line-by-line and executes them; if the filename is not valid, no execution takes place. 
 - `set varname = value`: The shell adds the `value:varname` pair to a symbol table, after which the user may substitute variables in commands by typing `$varname`. Variables may be substituted in commands, arguments, or `value`s in `set` commands themselves (but not in `varname`s). `varname` must start with either a letter or an underscore and may be followed by letters, digits, or underscores; `value` may be a string consisting of letters, digits, or spaces (if properly quoted). If the `varname` or `value` parameters are not valid, no variable setting occurs. Variables can be overwritten by subsequent `set` commands. The command syntax is sensitive to whitespace.
##### External Commands
KOSH supports (in theory) any command that is installed on a user's machine. 
##### Grammar
The KOSH grammar is, currently, a simplified version of that specified by POSIX. 
```
program				:	complete_commands 
					|	complete_commands '&'
                    
complete_commands	:	complete_commands ';' pipe_sequence redirect
					|	complete_commands ';' pipe_sequence 
                    |	pipe_sequence redirect
                    | 	pipe_sequence
                    
pipe_sequence		: 	pipe_sequence '|' command 
 					| 	command
                    
command				:	simple_command

simple_command		:	set_clause
					|	cmd_word arguments
                    |	cmd_word
                    
set_clause			:	"set" NAME '=' VALUE

cmd_word			:	WORD

arguments			:	arguments WORD
					|	WORD          

redirect			: 	'<' WORD
					|	'>' WORD
                    | 	'>>' WORD
					
```
##### Quoting
The following characters are important for KOSH operation and must be quoted to represent themselves:  
`$`, `|`, `;`, `&`.   
Quoting is, however, a work in progress and formal quoting syntax will be provided here when it is functional. 
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
The code is comprised the following files:  
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

The major methods in the AST class are (as of now) the constructors/destructors and the `expand_vars` methods, which attempt to expand any member strings with values from Shell's symbol table member. 

## Design choices
Before starting this project, I had very little idea of how a shell like BASH worked, how to manipulate the Linux environment with C++, or even how most of C++ worked. Once I had a handle on things, I initially started the development process by trying to make a POSIX-compliant shell, which immediately turned out to be a bad idea. I then made the following simplifications for myself (though these are subject to change as I continue working on the shell):
 * KOSH will be sensitive to whitespace in `set varname = value` commands. 
 * If possible, KOSH will not handle events asynchronously. (An exception is with the `fork()` and `exec()` maneuver found in `Shell::execute_external()`).
 * Nested constructs are forbidden, for example in `$(...)` constructs. 
 * Redirections to/from files may only occur at the end of a command (not before).
 
For the grammar, I based it largely on that specified by POSIX (since it was my starting point for learning shell operation), making accommodations for KOSH-specific functionality (for instance, we have no need for `&&` or `||` constructs, but we do have a particular `set` syntax).  

For the program itself, it largely evolved from my knowledge of Flex/Bison: I knew the program would need to parse input and that an effective way of operating on that input is to create an AST, so I started with the lexer and parser and based the AST on that.   
Of note about the KOSH AST interface is that each kind of node is a separate class, rather than a level in a hierarchy of classes as is typical. This choice was made largely because I had limited time to (re)learn C++ for this project, so I chose to focus on the aspects of the language I thought would be most immediately useful and simultaneously simple to implement, and unfortunately class inheritance was not one of those for me (whether this was a reasonable decision is entirely debatable). In addition, the relatively low number of distinct grammatical elements made it feasible to implement each as a separate class.  
In an early version of the project, I had the `execute` functionality embedded within the AST itself, which turned out to be incredibly unwieldy, so it was implemented in the Shell interface instead.

The Shell interface wasn't designed so much as pieced together based on what I needed to work next. First it was running `cd`, then it was `echo`, then the rest of the built-ins, and then I added the symbol table to accommodate variables, then external commands. 

On the whole, my code uses relatively little of the "advanced" facilities that C++ provides, such as templates or class hierarchies. This was partly a deliberate decision and partly a consequence of limited time and resources. I did, however, strive to make ample use of the STL; the `std::string` class forms the backbone of most of my text processing, and the `std::unordered_map` made the symbol table implementation a breeze. 

Exception handling is absent; I learned how it worked but didn't find the time to implement it, though I may decide to add it in as the code becomes more fleshed out.    

A note on the design of my classes: all class data is currently public. I know this is (very) bad form, this was meant to be a temporary decision to speed the development process and will be reverted shortly.
## Development Timeline 
I spent the period of time from early October to early November in what I call the "book phase": I researched how shells operate, compiler design, studied C++ textbooks and wrote small programs, read the documentation and practiced using the tools I would need for the project (Flex, Bison, GCC, GNU make), and studied Linux programming.

From early to mid November was the "design phase", where I laid out more or less what I actually needed for the KOSH project; the first versions of the program were made in this time. 

From mid-late November to early December was the "implementation phase", where the bulk of the current source code was produced. 
## Limitations / Known errors (WIP)
I'll try to keep this updated as I continue adding features to KOSH.

As of December 5th, 2018:
 * No pipes, redirections, etc. 
 * The `;` operator works in the following form: `command1 ; command2`. I haven't officially started implementing the `;` operator yet so this isn't really a bug as much as a half-feature for now.
 * At the `make` command, the compiler offers this complaint (it seems out of my control, but I'm sure there's some Flex/Bison hackery I could throw together when I have time):  
 	>  lex.yy.c:1225:17: warning: ‘void yyunput(int, char*)’ defined but not used [-Wunused-function]
    		 static void yyunput (int c, char * yy_bp )  


 * Flex warns that the rule at `shell.l:35` cannot be matched, but by trial and error I discovered that having this rule in place actually prevents a more serious lexing bug from occurring. Again, some Flex/Bison nitty-gritty ought to do the trick.
 * Not necessarily a bug, but the `set` clause is very picky about being space-separated.
 * The current `shell.l` file is not great; it's gone through very few touch-ups in the development process and future development should be contingent on getting a better and more robust scanner working.
 * No line history (will likely be added at some point). 
 * Current AST design is clunky. 
 * Exception handling is absent and error recovery is mostly supplied by Flex and Bison defaults which can be dodgy at times.
            
##### Flip side: Things I think went pretty well 
 All things considered, the shell works reasonably well at executing external commands and passing arguments. The variable mechanism is also surprisingly agile; for instance, you can set a variable to a filename and run `source $varname` and it executes as expected. You can even set a variable to another variable like so:  
 `set brandon = dog`  
 `set banana = $brandon`  
 and it sets `banana` to `dog`.
 
 Additionally, KOSH is often very good at managing allocated memory, with slight memory leaks occurring rather infrequently (usually due to Flex scanner errors). 
## Future Development 
Now that the deadline has passed, my medium-term goals for the project are:
1. Re-implement the scanner and parser, possibly reentrant, to handle errors more effectively and to handle more sophisticated language features. 
	* Specifically, I want command substitutions and piped commands to be executed in a sub-shell; this appears possible only with reentrant scanner/parser combinations. 
	* Additionally, I want to more rigorously define the mechanics for quoting and escaping special characters, and have a scanner/parser combo that can handle them sufficiently. 
	* All of the above will also require expanding the grammar. 
2. Get pipes, redirections, `;`s, and command substitution working, possibly using [this page](https://linux.die.net/man/7/pipe) as a starting point.
3. Implement effective error detection and handling.
## References (WIP)
###### C++ 
[Eric Roberts, *Programming Abstractions in C++*](https://www.pearson.com/us/higher-education/program/Roberts-Programming-Abstractions-in-C/PGM80147.html)    
[Bjarne Stroustrup, *The C++ Programming Language (4th ed)*](http://www.stroustrup.com/4th.html)  
[learncpp.com](https://www.learncpp.com/)    
[cplusplus.com](http://www.cplusplus.com/reference/) and [cppreference.com](https://en.cppreference.com/w/)    

###### Flex and Bison
[John Levine, *flex & bison*](http://shop.oreilly.com/product/9780596155988.do)   
[flex](https://westes.github.io/flex/manual/) and [bison](https://www.gnu.org/software/bison/manual/bison.html) documentations     
###### Linux Programming
[Mitchell et al, *Advanced Linux Programming*](https://mentorembedded.github.io/advancedlinuxprogramming/) 
[The Linux man pages](http://man7.org/linux/man-pages/) helped me learn how to program in the Linux environment. I also adapted the example from the [waitpid()](http://man7.org/linux/man-pages/man2/waitpid.2.html) page for my `Shell::execute_external()` method.   
###### Shell operation    
[POSIX Specification](http://pubs.opengroup.org/onlinepubs/9699919799/)     
[*Advanced Bash Scripting*](http://tldp.org/LDP/abs/html/)
