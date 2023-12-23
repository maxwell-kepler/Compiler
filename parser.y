%{ 
    #include <stdio.h> 
    #include <string.h>
    #include "scanner.h" //#include "token.h" 
    #include "SA.h"
    
    struct declaration *parser_result = 0;
    int yyerror(const char *msg);
    int yylex(void);

%} 
%define parse.error verbose

%token TOKEN_ELSE TOKEN_IF TOKEN_INT TOKEN_RETURN TOKEN_VOID TOKEN_WHILE
%token TOKEN_ADD TOKEN_MINUS TOKEN_MUL TOKEN_DIV
%token TOKEN_LESS_EQ TOKEN_LESS TOKEN_MORE_EQ TOKEN_MORE 
%token TOKEN_EQ_COMP TOKEN_EQ TOKEN_SEMI TOKEN_COMMA
%token TOKEN_PAREN_OP TOKEN_PAREN_CL TOKEN_CURLY_OP TOKEN_CURLY_CL TOKEN_SQUARE_OP TOKEN_SQUARE_CL
%token TOKEN_ID TOKEN_NUM
%token TOKEN_ERROR

%left "="
%right "then" TOKEN_ELSE 

%union {
    struct declaration *declaration;
    struct var_declaration *var_declaration;
    struct type_specifier *type_specifier;
    struct fun_declaration *fun_declaration;
    struct param *param;
    struct compound_stmt *compound_stmt;
    struct local_declarations *local_declarations;
    struct statement_list *statement_list;
    struct statement *statement;
    struct expression_stmt *expression_stmt;
    struct selection_stmt *selection_stmt;
    struct iteration_stmt *iteration_stmt;
    struct return_stmt *return_stmt;
    struct expression *expression;
    struct var *var;
    struct simple_expression *simple_expression;
    struct relop *relop;
    struct additive_expression *additive_expression;
    struct addop *addop;
    struct term *term;
    struct mulop *mulop;
    struct factor *factor;
    struct call *call;
    struct args *args;
    struct arg_list *arg_list;
    char *id;
    int num;
}

%type<declaration> program declaration_list declaration
%type<var_declaration> var_declaration
%type<type_specifier> type_specifier
%type<fun_declaration> fun_declaration
%type<param> params param_list param
%type<compound_stmt> compound_stmt
%type<local_declarations> local_declarations
%type<statement_list> statement_list
%type<statement> statement
%type<expression_stmt> expression_stmt
%type<selection_stmt> selection_stmt
%type<iteration_stmt> iteration_stmt
%type<return_stmt> return_stmt
%type<expression> expression
%type<var> var
%type<simple_expression> simple_expression
%type<relop> relop
%type<additive_expression> additive_expression
%type<addop> addop
%type<term> term
%type<mulop> mulop
%type<factor> factor
%type<call> call
%type<args> args
%type<arg_list> arg_list
%type<id> TOKEN_ID
%type<num> TOKEN_NUM

%%
//Rule 1: program -> declaration-list
program : declaration_list {parser_result = $1;}


//Rule 2: declaration-list -> declaration-list declaration | declaration
//Modification: declaration-list -> declaration declaration-list | declaration
//This ensures that the declarations appear in the correct order
declaration_list : declaration declaration_list {$$ = $1; $1->next=$2;}
    | declaration {$$ = $1;}
    

//Rule 3: declaration -> var-declaration | fun-declaration
declaration : var_declaration {$$ = createDecl(VAR, $1, 0, 0);}
    | fun_declaration {$$ = createDecl(FUNC, 0, $1, 0);}


//Rule 4: var-declaration -> type-specifier ID ; | type-specifier ID [ NUM ] ;
var_declaration : type_specifier TOKEN_ID TOKEN_SEMI {$$ = createVarDecl($1, $2, 0);} 
    | type_specifier TOKEN_ID TOKEN_SQUARE_OP TOKEN_NUM TOKEN_SQUARE_CL TOKEN_SEMI {$$ = createVarDecl($1, $2, $4);} 


//Rule 5: type-specifier -> int | void
type_specifier : TOKEN_INT {$$ = createType(INT_TYPE);}
    | TOKEN_VOID {$$ = createType(VOID_TYPE);}
    

//Rule 6: fun-declaration -> type-specifier ID ( params) compound-stmt
fun_declaration : type_specifier TOKEN_ID TOKEN_PAREN_OP params TOKEN_PAREN_CL compound_stmt {$$ = createFuncDecl($1, $2, $4, $6);} 


//Rule 7: params -> param-list | void
params : param_list {$$ = $1;}
    | TOKEN_VOID {$$ = createParam(createType(VOID_TYPE), 0, false, 0);}
    

//Rule 8: param-list -> param-list , param | param
//Modification: param-list -> param , param-list | param
//This ensures that the params appear in the correct order
param_list : param TOKEN_COMMA param_list {$$ = $1; $1->next=$3;}
    | param {$$ = $1;}
    

//Rule 9: param -> type-specifier ID | type-specifier ID []
param : type_specifier TOKEN_ID {$$ = createParam($1, $2, false, 0);}
    | type_specifier TOKEN_ID TOKEN_SQUARE_OP TOKEN_SQUARE_CL {$$ = createParam($1, $2, true, 0);}
    

//Rule 10: compound-stmt -> { local-declarations statement-list }
//Modification: compound_stmt -> {} | {local_declarations} | {statement_list} | {local_declarations statement_list}
//This ensures that rule 11 and 12 don't have the empty word as this parser isn't reading it properly
compound_stmt : TOKEN_CURLY_OP TOKEN_CURLY_CL {$$ = createCompoundStmt(0, 0);}
    | TOKEN_CURLY_OP local_declarations TOKEN_CURLY_CL {$$ = createCompoundStmt($2, 0);}
    | TOKEN_CURLY_OP statement_list TOKEN_CURLY_CL {$$ = createCompoundStmt(0, $2);}
    | TOKEN_CURLY_OP local_declarations statement_list TOKEN_CURLY_CL {$$ = createCompoundStmt($2, $3);}


//Rule 11: local-declarations -> local-declarations var-declaration | empty
//Modification: local_declarations -> var_declaration | var_declaration local_declarations 
//This is due to rule 10 removing the need for empty
local_declarations : var_declaration {$$ = createLocalDecl($1, 0);}
    | var_declaration local_declarations  {$$ = createLocalDecl($1, $2);}
    

//Rule 12: statement-list -> statement-list statement | empty
//Modification: statement_list -> statement | statement statement_list
//This is due to rule 10 removing the need for empty
statement_list : statement {$$ = createStatementList($1, 0);}
    | statement statement_list {$$ = createStatementList($1, $2);}


//Rule 13: statement -> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt
statement : expression_stmt {$$ = createStatement($1, 0, 0, 0, 0);} 
    | compound_stmt         {$$ = createStatement(0, $1, 0, 0, 0);}
    | selection_stmt        {$$ = createStatement(0, 0, $1, 0, 0);}
    | iteration_stmt        {$$ = createStatement(0, 0, 0, $1, 0);}
    | return_stmt           {$$ = createStatement(0, 0, 0, 0, $1);}
    

//Rule 14: expression-stmt -> expression ; | ;
expression_stmt : expression TOKEN_SEMI {$$ = createExpressionStatement($1);}
    | TOKEN_SEMI {$$ = createExpressionStatement(0);}
    

//Rule 15: selection-stmt -> if ( expression ) statement | if ( expression ) statement else statement
//https://stackoverflow.com/questions/12731922/reforming-the-grammar-to-remove-shift-reduce-conflict-in-if-then-else
selection_stmt : TOKEN_IF TOKEN_PAREN_OP expression TOKEN_PAREN_CL statement    %prec "then" {$$ = createSelectionStmt($3, $5, 0);}
    | TOKEN_IF TOKEN_PAREN_OP expression TOKEN_PAREN_CL statement TOKEN_ELSE statement {$$ = createSelectionStmt($3, $5, $7);}
    

//Rule 16: iteration-stmt -> while ( expression ) statement
iteration_stmt : TOKEN_WHILE TOKEN_PAREN_OP expression TOKEN_PAREN_CL statement {$$ = createIterationStmt($3, $5);}


//Rule 17: return-stmt -> return ; | return expression ;
return_stmt : TOKEN_RETURN TOKEN_SEMI {$$ = createReturnStmt(0);}
    | TOKEN_RETURN expression TOKEN_SEMI {$$ = createReturnStmt($2);}
    

//Rule 18: expression -> var = expression | simple-expression
expression : var "=" expression {$$ = createExpr($1, $3, 0);}
    | simple_expression {$$ = createExpr(0, 0, $1);}
    

//Rule 19: var -> ID | ID [ expression ]
var : TOKEN_ID {$$ = createVar($1, 0);}
    | TOKEN_ID TOKEN_SQUARE_OP expression TOKEN_SQUARE_CL {$$ = createVar($1, $3);}


//Rule 20: simple-expression -> additive-expression relop additive-expression | additive-expression
simple_expression : additive_expression relop additive_expression {$$ = createSimpleExpr($1, $2, $3);}
    | additive_expression {$$ = createSimpleExpr($1, 0, 0);}


//Rule 21
relop : TOKEN_LESS_EQ {$$ = createRelop(LESS_EQ);}
    | TOKEN_LESS {$$ = createRelop(LESS);}
    | TOKEN_MORE_EQ {$$ = createRelop(MORE_EQ);}
    | TOKEN_MORE {$$ = createRelop(MORE);}
    | TOKEN_EQ_COMP {$$ = createRelop(EQ_COMP);}
    | TOKEN_EQ {$$ = createRelop(EQ);}
    

//Rule 22
additive_expression : additive_expression addop term {$$ = createAdditiveExpression($1, $2, $3);}
    | term {$$ = createAdditiveExpression(0, 0, $1);}
    

//Rule 23
addop : TOKEN_ADD {$$ = createAddop(ADD);}
    | TOKEN_MINUS {$$ = createAddop(MINUS);}
    

//Rule 24
term : term mulop factor {$$ = createTerm($1, $2, $3);}
    | factor {$$ = createTerm(0, 0, $1);}
    

//Rule 25
mulop : TOKEN_MUL {$$ = createMulop(MUL);}
    | TOKEN_DIV {$$ = createMulop(DIV);}
    

//Rule 26: factor -> ( expression ) | var | call | NUM
factor : TOKEN_PAREN_OP expression TOKEN_PAREN_CL {$$ = createFactor($2, 0, 0, 0);}
    | var {$$ = createFactor(0, $1, 0, 0);}
    | call {$$ = createFactor(0, 0, $1, 0);}
    | TOKEN_NUM {$$ = createFactor(0, 0, 0, $1);}
    

//Rule 27: call -> ID ( args )
//Modification: call -> ID () | ID ( args )
//This ensures that rule 28 doesn't have the empty word as this parser isn't reading it properly
call : TOKEN_ID TOKEN_PAREN_OP TOKEN_PAREN_CL {$$ = createCall($1, 0);}
    | TOKEN_ID TOKEN_PAREN_OP args TOKEN_PAREN_CL {$$ = createCall($1, $3);}


//Rule 28: args -> arg-list | empty
//Modification: args -> arg-list
//This is due to rule 27 removing the need for empty
args : arg_list {$$ = createArgs($1);}


//Rule 29: arg-list -> arg-list , expression | expression
//Modification: arg-list -> expression , arg-list  | expression
//This ensures that the args appear in the correct order
arg_list : expression TOKEN_COMMA arg_list {$$ = createArgList($3, $1);}//arg_list TOKEN_COMMA expression {$$ = createArgList($1, $3);}
    | expression {$$ = createArgList(0, $1);}
    

%% 
extern FILE *yyin; 
extern int yyparse(); 
extern char *yytext; 


int main(int argc, char **argv){
    //char filePath[] = "testFiles/";
    //char* filename = "exampleInput.c";
    //extern bool debugMode;
    /*if(argv [2] != NULL) {
        filename = argv[1];
        debugMode = true;
    }
    else if(argv[1] != NULL) {
        debugMode = false;
        if (strcmp(argv[1], "--DEBUG"))
            filename = argv[1];
        else 
            debugMode = true;
    }
    strcat(filePath, filename);
    yyin = fopen(filePath,"r"); 
    if(!yyin) { 
        printf("could not open %s!\n", filename); 
        return 1; 
    } */
    extern bool debugMode;
    debugMode = false;
    if(argc > 1){
        yyin = fopen(argv[1], "r");
        if(!yyin){
            printf("error reading the file: %s\n", argv[1]);
            return 1;
        }
    }else {
        printf("usage: ./parser <filename>");
        return 1;
    }
    int result = yyparse();
    if(result == 0){
        printDecl(parser_result);
        printf("\n\n\nParsing succeeded\n");
        ast_help();
        decl_resolve(parser_result);
        printf("\n\n\nValid grammar!\n");
    }else {
        printf("\nParsing failed\n");
    }
    return result;
}


int yyerror(const char *msg){
    extern char* yytext;
    printf("ERROR: %s at symbol %s ", msg, yytext);
    return 1;
}