# README for CPSC 411 - Assignment 3 - Semantic Analyzer for C-

## Maxwell Kepler 30093950

## NEW CHANGES
scanner.l: 
- changed #include "token.h" to #include "parser.h"
parser.y: 
- changed #include "token.h" to #include "scanner.h" 
- changed how the files were being read 
- hardcoded debugging to false instead of being read in from the command line
ast.h
- added check to print statements that expression exists before printing to remove segmentation fault

how to run the code:
1.      flex --header-file=scanner.h -o scanner.c scanner.l
2.      bison -dvo parser.c parser.y
3.      gcc scanner.c parser.c
4.      ./a.out filepath

Everything above this are the changes I've made
////////////////////////////////////////////////////////////////////////////////


















## Instructions on how to run the project

Below are the commands to compile and run the project.

1. flex -o scanner.c scanner.l
2. bison --defines=token.h -o parser.c parser.y
3. gcc scanner.c parser.c
4. ./a optional_file_name optional_debug_flag

The main method is stored at the bottom of parser.y

## How the output can be interpreted

If the parser succeeds, the valid AST is printed to the screen.
If the semantic analyzer succeeds, it will print the stack at different key moments. As well, it will print information of a variable when it is used. If it fails, it will print an error explaing the failure. There are two optional parameters that can be passed. The first is _optional_file_name_, which can be found from the list below. The other is _optional_debug_flag_, indicated with _--DEBUG_. This causes debug messages to be printed to the screen. Ensure that the filename is passed first if both are passed. As well, a helper function is used that adds the input and output functions on to the stack. This is an easier way to ensure that the types match up.

## Explanation of what the test cases are and what they are testing for

The test cases are stored in the folder called testFiles. The default is exampleInput.c, which was given in the assignment handout. The rest of the test cases, label test1.c to test12.c are from the assignment 3 handout:

-   exampleInput.c
    -   The default input, given in the assignments handout.
-   test1.c
    -   This case should cause an error.
    -   This test cases ensures that an identity must be declared before use.
-   test2.c
    -   This case should pass.
    -   This case tests redeclarations in from different scopes, having foo be a global function, but being redeclared in the main function as an integer variable.
-   test3.c
    -   This case should cause an error.
    -   This test redeclares integer x in the same scope.
-   test4.c
    -   This case should pass.
    -   This tests that a variable in the parameter can have a value assigned to it.
-   test5.c
    -   This case should cause an error.
    -   This tests that a parameter doesn't get redeclared.
-   test6.c
    -   This case should cause an error.
    -   This tests that the last global declaration is the main function.
-   test7.c
    -   This case should cause an error.
    -   This tests that additive expressions are between integers.
-   test7-1.c
    -   This case should cause an error.
    -   This tests that mulops are between integers.
-   test7-2.c
    -   This case should cause an error.
    -   This tests that relops are between integers.
-   test7-3.c
    -   This case should cause an error.
    -   This tests that assignment is between integers.
-   test8.c
    -   This case should cause an error.
    -   This tests the incorrect use of the function call syntax.
-   test9.c
    -   This case should cause an error.
    -   This tests the incorrect use of array index syntax.
-   test10.c
    -   This case should cause an error.
    -   This tests that the arguments and paramaters are of the same type.
-   test10-1.c
    -   This case should cause an error.
    -   This tests that there must be the same number of arguments and parameters (not enough given).
-   test10-2.c
    -   This case should cause an error.
    -   This tests that there must be the same number of arguments and parameters (too many given).
-   test10-3.c
    -   This case should cause an error.
    -   This tests that a number can't be passed as an array.
-   test11.c
    -   This case should cause an error.
    -   This tests that the return value is of the same type as the function.
-   test12.c
    -   This case should cause an error.
    -   This tests that the return statement can't be empty.

## How the semantic analyzer was created

I built this on top of the parser from the last assignment. Still no working line numbers. There now is a debugMode variable that, when set to true, will add more print statements, making it easier to debug the code. This code was partially based on the semantic analyzer from the text book.

## Changes to the C- grammar to an equivalent grammar

As I implemented the parser using BISON, I changed the C- grammar so that it would work.
The rules I changed are as follows:

-   Rule 2
    -   Original: declaration-list -> declaration-list declaration | declaration
    -   Modification: declaration-list -> declaration declaration-list | declaration
    -   Reasoning: This ensures that the declarations appear in the correct order
-   Rule 8
    -   Original: param-list -> param-list , param | param
    -   Modification: param-list -> param , param-list | param
    -   Reasoning: This ensures that the params appear in the correct order
-   Rule 10
    -   Original: compound-stmt -> { local-declarations statement-list }
    -   Modification: compound_stmt -> {} | {local_declarations} | {statement_list} | {local_declarations statement_list}
    -   Reasoning: This ensures that rule 11 and 12 don't have the empty word as this parser isn't reading it properly
-   Rule 11
    -   Original: local-declarations -> local-declarations var-declaration | empty
    -   Modification: local_declarations -> var_declaration | var_declaration local_declarations
    -   Reasoning: This is due to rule 10 removing the need for empty
-   Rule 12
    -   Original: statement-list -> statement-list statement | empty
    -   Modification: statement_list -> statement | statement statement_list
    -   Reasoning: This is due to rule 10 removing the need for empty
-   Rule 27
    -   Original: call -> ID ( args )
    -   Modification: call -> ID () | ID ( args )
    -   Reasoning: This ensures that rule 28 doesn't have the empty word as this parser isn't reading it properly
-   Rule 28
    -   Original: args -> arg-list | empty
    -   Modification: args -> arg-list
    -   Reasoning: This is due to rule 27 removing the need for empty
-   Rule 29
    -   Original: arg-list -> arg-list , expression | expression
    -   Modification: arg-list -> expression , arg-list | expression
    -   Reasoning: This ensures that the args appear in the correct order
