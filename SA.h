#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"

/////GLOBAL VARIABLES FOR SEMANTIC ANALYZER////////////////////////////////////
int current_scope = 0;
struct node *head = NULL;
bool global_main = false;
bool debugMode;

/////SEMANTIC ANALYZER/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////SYMBOL////////////////////////////////////////////////////////////////////
typedef enum
{
    SCOPE_LOCAL,
    SCOPE_PARAM,
    SCOPE_GLOBAL,
} symbol_scope;

struct symbol
{
    decl_type declType; // VAR, FUNC
    symbol_scope scope;
    char *ident;
    typeKind kind; // INT_TYPE, VOID_TYPE
    bool hasArray;
};

struct symbol *createSymbol(decl_type declType, symbol_scope scope, char *id, typeKind kind, bool hasArray)
{
    struct symbol *newSymbol = malloc(sizeof *newSymbol);
    newSymbol->declType = declType;
    newSymbol->scope = scope;
    newSymbol->ident = id;
    newSymbol->kind = kind;
    newSymbol->hasArray = hasArray;
    return newSymbol;
}

///////////////////////////////////////////////////////////////////////////////
/////LINKED LIST FOR SEMANTIC ANALYZER/////////////////////////////////////////
void printList(struct node *node);
struct node
{
    struct symbol *Symbol;
    struct node *next;
};

// Insert at the end of the list
void insert(struct node **head_ref, struct symbol *new_data)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    struct node *last_node = *head_ref;
    new_node->Symbol = new_data;
    new_node->next = NULL;
    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }
    while (last_node->next != NULL)
        last_node = last_node->next;
    last_node->next = new_node;
    return;
}

void scope_enter() { current_scope++; }
void scope_exit(struct node **head_ref)
{
    struct node *last_valid_node = *head_ref, *prev;
    bool exist = false;
    struct node *safety_check = *head_ref;
    while (safety_check != NULL)
    {
        if (safety_check->Symbol->scope == current_scope)
            exist = true;
        safety_check = safety_check->next;
    }
    if (exist == false)
    {
        current_scope = 0;
        return;
    }
    while (last_valid_node->Symbol->scope != current_scope)
    {
        prev = last_valid_node;
        last_valid_node = last_valid_node->next;
    }
    prev->next = NULL;
    free(last_valid_node);
    current_scope = 0;
}
int scope_lookup(struct node *node, char *id)
{
    // Tries to find the last declaration of the id, returning the scope value
    // If unsuccessful, returns -1
    int scope_node = -1;
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id))
            scope_node = node->Symbol->scope;
        node = node->next;
    }
    return scope_node;
}
typeKind scope_lookup_kind(struct node *node, char *id)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    typeKind kind;
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id))
            kind = node->Symbol->kind;
        node = node->next;
    }
    return kind;
}

decl_type scope_lookup_declkind(struct node *node, char *id)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    decl_type declType;
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id))
            declType = node->Symbol->declType;
        node = node->next;
    }
    return declType;
}

bool scope_lookup_hasArray(struct node *node, char *id)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    bool hasArray = false;
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id))
            hasArray = node->Symbol->hasArray;
        node = node->next;
    }
    return hasArray;
}

int getNumParams(struct node *node, char *id)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    int count = 0;
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id) && node->Symbol->scope == 0)
            break;
        node = node->next;
    }
    node = node->next;
    while (node != NULL)
    {
        if (node->Symbol->scope != 1)
            break;
        count++;
        node = node->next;
    }
    return count;
}

bool getParamByValue(struct node *node, char *id, int offset)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id) && node->Symbol->scope == 0)
            break;
        node = node->next;
    }
    node = node->next;
    for (int i = 0; i < offset; i++)
    {
        node = node->next;
    }
    return node->Symbol->hasArray;
}

char *getParamNAMEByValue(struct node *node, char *id, int offset)
{
    int value = scope_lookup(node, id);
    if (value == -1)
    {
        printf("Node %s was not found\n", id);
    }
    while (node != NULL)
    {
        if (!strcmp(node->Symbol->ident, id) && node->Symbol->scope == 0)
            break;
        node = node->next;
    }
    node = node->next;
    for (int i = 0; i < offset; i++)
    {
        node = node->next;
    }
    return node->Symbol->ident;
}

char *getNodeDeclType(struct symbol *Symbol)
{
    switch (Symbol->declType)
    {
    case VAR:
        return "variable";
    case FUNC:
        return "function";
    default:
        break;
    }
}

char *getNodeKindType(typeKind kind)
{
    switch (kind)
    {
    case INT_TYPE:
        return "int";
    case VOID_TYPE:
        return "void";
    default:
        break;
    }
}

char *getScope(int scope)
{
    switch (scope)
    {
    case 0:
        return "GLOBAL";
    case 1:
        return "PARAM";
    case 2:
        return "LOCAL";
    default:
        break;
    }
}

void printList(struct node *node)
{
    while (node != NULL)
    {
        char *type = getNodeDeclType(node->Symbol); // VAR, FUNC
        int scope = node->Symbol->scope;
        char *scopeName = getScope(scope);
        char *ident = node->Symbol->ident;
        char *kind = getNodeKindType(node->Symbol->kind); // int, void
        bool hasArr = node->Symbol->hasArray;
        printf("%6s: %8s | %-4s | %-6s", ident, type, kind, scopeName);
        if (hasArr)
        {
            printf(" | []");
        }
        printf("\n");
        node = node->next;
    }
}

char *lastNode(struct node *node)
{
    while (node != NULL)
    {
        if (node->next == NULL)
            return node->Symbol->ident;
        node = node->next;
    }
}

typeKind lastGlobalFuncType(struct node *node)
{
    typeKind kind;
    while (node != NULL)
    {
        if (node->Symbol->scope == 0)
            kind = node->Symbol->kind;
        node = node->next;
    }
    return kind;
}

char *lastGlobalFuncName(struct node *node)
{
    char *id;
    while (node != NULL)
    {
        if (node->Symbol->scope == 0)
            id = node->Symbol->ident;
        node = node->next;
    }
    return id;
}
void ast_help();
void decl_resolve(struct declaration *decl);
// Helper function to ensure that the C- input and output functions are in the linked list for future reference
void ast_help()
{
    struct symbol *inputSymbol = createSymbol(FUNC, 0, "input", INT_TYPE, false);
    insert(&head, inputSymbol);
    struct symbol *outputSymbol = createSymbol(FUNC, 0, "output", VOID_TYPE, false);
    insert(&head, outputSymbol);
}

/////RESOLVE FUNCTION DECLARATIONS/////////////////////////////////////////////
void decl_resolve(struct declaration *decl);
void varDecl_resolve(struct var_declaration *varDecl);
void funcDecl_resolve(struct fun_declaration *funcDecl);
void params_resolve(struct param *Param);
void compoundStmt_resolve(struct compound_stmt *compound_stmt);
void localDecl_resolve(struct local_declarations *local_declarations);
void statementList_resolve(struct statement_list *statement_list);
void statement_resolve(struct statement *statement);
void expressionStatement_resolve(struct expression_stmt *expression_stmt);
void selectionStmt_resolve(struct selection_stmt *selection_stmt);
void iterationStmt_resolve(struct iteration_stmt *iteration_stmt);
void returnStmt_resolve(struct return_stmt *return_stmt);
void expression_resolve(struct expression *expression);
void var_resolve(struct var *var);
void simpleExpression_resolve(struct simple_expression *simple_expression);
void additiveExpression_resolve(struct additive_expression *additive_expression);
void term_resolve(struct term *term);
void factor_resolve(struct factor *factor);
void call_resolve(struct call *call);
void args_resolve(struct args *args);
void argList_resolve(struct arg_list *arg_list);

void typeCheckTERMS(struct term *term_LHS, struct term *term_RHS, char *symbol);
void typeCheckFACTORS(struct factor *factor_LHS, struct factor *factor_RHS, char *symbol);
void typeCheckARRAYS(struct expression *expression, char *id);
void typeCheckPARAMS(struct arg_list *index, char *call_id, int arg_count);
/////RESOLVE FUNCTIONS/////////////////////////////////////////////////////////
void decl_resolve(struct declaration *decl)
{
    if (!decl)
    {
        char *last = lastNode(head);
        if (strcmp(last, "main"))
        {
            printf("[ERROR] LAST GLOBAL DECLARATION IS NOT MAIN FUNCTION, INSTEAD FOUND: %s\n", last);
            exit(1);
        }
        return;
    }

    if (decl->declType == VAR)
        varDecl_resolve(decl->var_declaration);
    if (decl->declType == FUNC)
        funcDecl_resolve(decl->fun_declaration);

    printf("[DECLARATIONS] VAR/FUNC WAS FULLY DEFINED\n");
    printList(head);
    decl_resolve(decl->next);
}

void varDecl_resolve(struct var_declaration *varDecl)
{
    char *id = varDecl->id;
    bool hasArray = varDecl->value;

    struct symbol *currentSymbol = createSymbol(VAR, current_scope, id, varDecl->type_specifier->kind, hasArray);
    int potential_scope = scope_lookup(head, id);
    if (potential_scope == current_scope)
    {
        printf("[ERROR] REDECLARATION IN THE SAME SCOPE: %s\n", id);
        exit(1);
    }
    if (potential_scope == 1 && current_scope == 2)
    {
        printf("[ERROR] PARAMETER SCOPE REDECLARATION: %s\n", id);
        exit(1);
    }

    insert(&head, currentSymbol);
}

void funcDecl_resolve(struct fun_declaration *funcDecl)
{
    // Add function name to stack
    char *id = funcDecl->id;
    struct symbol *currentSymbol = createSymbol(FUNC, current_scope, id, funcDecl->type_specifier->kind, false);
    insert(&head, currentSymbol);
    printf("[DECLARATIONS] BEFORE FUNC %s IS DEFINED \n", id);
    printList(head);
    // Increase the scope
    scope_enter();

    // Handle params
    params_resolve(funcDecl->param);

    scope_enter();
    // Handle compound statement
    struct compound_stmt *cmpStmt = funcDecl->compound_stmt;
    if (cmpStmt)
        compoundStmt_resolve(cmpStmt);

    // Leave the scope
    printf("[DECLARATIONS] BEFORE THE SCOPE OF FUNC %s IS LEFT \n", id);
    printList(head);
    scope_exit(&head);
}

void params_resolve(struct param *Param)
{
    if (!Param)
    {
        printf("[DECLARATIONS] PARAMS ARE DECLARED\n");
        printList(head);
        return;
    }
    char *paramType = getType(Param->type_specifier);
    char *id = Param->id;
    if (!(strcmp(paramType, "void")) && (id == NULL))
        return; // only param is void, don't add to stack
    struct symbol *currentSymbol = createSymbol(VAR, current_scope, id, Param->type_specifier->kind, Param->hasArray);
    insert(&head, currentSymbol);
    params_resolve(Param->next);
}

void compoundStmt_resolve(struct compound_stmt *compound_stmt)
{
    localDecl_resolve(compound_stmt->local_declarations);
    statementList_resolve(compound_stmt->statement_list);
}

void localDecl_resolve(struct local_declarations *local_declarations)
{
    if (!local_declarations)
        return;
    varDecl_resolve(local_declarations->var_declaration);
    localDecl_resolve(local_declarations->next);
}

void statementList_resolve(struct statement_list *statement_list)
{
    if (!statement_list)
        return;
    statement_resolve(statement_list->statement);
    statementList_resolve(statement_list->next);
}

void statement_resolve(struct statement *statement)
{
    if (statement->expression_stmt)
        expressionStatement_resolve(statement->expression_stmt);

    if (statement->compound_stmt)
        compoundStmt_resolve(statement->compound_stmt);

    if (statement->selection_stmt)
        selectionStmt_resolve(statement->selection_stmt);

    if (statement->iteration_stmt)
        iterationStmt_resolve(statement->iteration_stmt);

    if (statement->return_stmt)
        returnStmt_resolve(statement->return_stmt);
}

void expressionStatement_resolve(struct expression_stmt *expression_stmt)
{
    if (debugMode) printf("[DEBUG -- CALL] expression_resolve FROM expressionStatement_resolve\n");
    expression_resolve(expression_stmt->expression);
}

void selectionStmt_resolve(struct selection_stmt *selection_stmt)
{
    expression_resolve(selection_stmt->expression);
    statement_resolve(selection_stmt->then_statement);
    struct statement *else_statement = selection_stmt->else_statement;
    if (else_statement) statement_resolve(else_statement);
}

void iterationStmt_resolve(struct iteration_stmt *iteration_stmt)
{
    expression_resolve(iteration_stmt->expression);
    statement_resolve(iteration_stmt->statement);
}

void returnStmt_resolve(struct return_stmt *return_stmt)
{
    struct expression *expression = return_stmt->expression;
    typeKind kind = lastGlobalFuncType(head);
    if (expression && kind == VOID_TYPE)
    {
        printf("[ERROR] RETURNING VALUE FROM A VOID FUNCTION: %s\n", lastGlobalFuncName(head));
        exit(1);
    }
    if (!expression)
    {
        printf("[ERROR] EMPTY RETURN IN %s FUNCTION: %s\n", getNodeKindType(kind), lastGlobalFuncName(head));
        exit(1);
    }
    expression_resolve(return_stmt->expression);
}

void expression_resolve(struct expression *expression)
{
    struct var *var = expression->var;
    struct expression *nextExp = expression->expression;
    struct simple_expression *simple_expression = expression->simple_expression;
    if (var)
    {
        if (debugMode) printf("[DEBUG -- CALL] var_resolve FROM expression_resolve\n");
        var_resolve(var);
        expression_resolve(expression);
    }
    else
    {
        if (debugMode) printf("[DEBUG -- CALL] simpleExpression_resolve FROM expression_resolve\n");
        simpleExpression_resolve(simple_expression);
    }
}

void var_resolve(struct var *var)
{
    char *id = var->id;
    int found_scope = scope_lookup(head, id);
    if (found_scope == -1)
    {
        printf("[ERROR] DECLARATION BEFORE USE: %s\n", id);
        exit(1);
    }
    typeKind kind = scope_lookup_kind(head, id);
    bool hasArray = scope_lookup_hasArray(head, id);
    printf("[VALUE] FROM var_resolve %s: VAR | %s | %s", id, getNodeKindType(kind), getScope(found_scope));
    if (hasArray)
        printf(" | []");
    printf("\n");

    if (var->expression)
    {
        expression_resolve(var->expression); // Ensure things are good
        if (debugMode) printf("[DEBUG -- CHECK: TYPECHECK] ARRAYS FROM var_resolve\n");
        typeCheckARRAYS(var->expression, id);
    }
}

void simpleExpression_resolve(struct simple_expression *simple_expression)
{
    struct additive_expression *additive_expression_left = simple_expression->additive_expression_left;
    struct additive_expression *additive_expression_right = simple_expression->additive_expression_right;
    if (debugMode) printf("[DEBUG -- CALL] additiveExpression_resolve [ left ] FROM simpleExpression_resolve\n");
    additiveExpression_resolve(additive_expression_left);
    if (additive_expression_right)
    {
        additiveExpression_resolve(additive_expression_right);
        if (debugMode) printf("[DEBUG -- CALL] additiveExpression_resolve [ right ] FROM simpleExpression_resolve\n");
    }

    // Method to validate types with relop
    if (simple_expression->relop)
    {
        if (additive_expression_left->term && additive_expression_right->term)
        {
            if (debugMode) printf("[DEBUG -- CHECK: TYPECHECK] TERMS FROM simpleExpression_resolve\n");
            // validate relop
            char *symbol = getRelopType(simple_expression->relop);
            typeCheckTERMS(additive_expression_left->term, additive_expression_right->term, symbol);
        }
    }
}

void additiveExpression_resolve(struct additive_expression *additive_expression)
{
    struct additive_expression *additive_expression_LHS = additive_expression->additive_expression;
    struct term *term = additive_expression->term;
    struct addop *addop = additive_expression->addop;
    if (addop)
    {
        if (debugMode) printf("[DEBUG -- CALL] additiveExpression_resolve FROM additiveExpression_resolve\n");
        additiveExpression_resolve(additive_expression_LHS);
    }
    if (debugMode) printf("[DEBUG -- CALL] term_resolve FROM additiveExpression_resolve\n");
    term_resolve(term);

    // Method to validate types with addop
    if (addop)
    {
        // Ensure elements exists, else exit
        struct term *term_LHS = additive_expression_LHS->term;
        if (!term_LHS)
        {
            if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] term_LHS IN additiveExpression_resolve\n");
            return;
        }

        if (debugMode) printf("[DEBUG -- CHECK: TYPECHECK] TERMS FROM additiveExpression_resolve\n");
        // validate addop
        char *symbol = getAddopType(addop);
        typeCheckTERMS(term_LHS, term, symbol);
    }
}

void term_resolve(struct term *term)
{
    struct term *term_LHS = term->term;
    struct factor *factor = term->factor;
    if (term_LHS)
    {
        if (debugMode) printf("[DEBUG -- CALL] term_resolve FROM term_resolve\n");
        term_resolve(term_LHS);
    }
    if (debugMode) printf("[DEBUG -- CALL] factor_resolve FROM term_resolve\n");
    factor_resolve(factor);
    if (term->mulop)
    {
        if (debugMode) printf("[DEBUG - CHECK: TYPECHECK] TERMS FROM term_resolve\n");
        typeCheckTERMS(term_LHS, term, getMulopType(term->mulop)); // validate mulop
    }
}

void factor_resolve(struct factor *factor)
{
    struct expression *expression = factor->expression;
    if (expression)
    {
        if (debugMode) printf("[DEBUG -- CALL] expression_resolve FROM factor_resolve\n");
        expression_resolve(expression);
        return;
    }
    struct var *var = factor->var;
    if (var)
    {
        if (debugMode) printf("[DEBUG -- CALL] var_resolve FROM factor_resolve\n");
        var_resolve(var);
        return;
    }
    struct call *call = factor->call;
    if (call)
    {
        if (debugMode) printf("[DEBUG -- CALL] call_resolve FROM factor_resolve\n");
        call_resolve(call);
        return;
    }
    int value = factor->value;
    if (debugMode) printf("[DEBUG - VALUE] factor_resolve: int | %d\n", value);
    if (!debugMode) printf("[VALUE] FROM factor_resolve: int | %d\n", value);
}

void call_resolve(struct call *call)
{
    char *call_id = call->id;
    struct args *args = call->args;
    typeKind kind = scope_lookup_kind(head, call_id);
    char *kindSymbol = getNodeKindType(kind);
    printf("[VALUE] FROM call_resolve %s: %s\n", call_id, kindSymbol);
    decl_type declType = scope_lookup_declkind(head, call_id);
    if (declType == VAR)
    {
        printf("[ERROR] INCORRECT USE OF FUNCTION CALL SYNTAX: %s\n", call_id);
        exit(1);
    }

    if (args)
        args_resolve(args);
    // attempt to ensure function call parameters are correct
    if (!args)
        return;

    int arg_count = 0;
    int numParams = getNumParams(head, call_id);
    if (numParams)
    {
        struct arg_list *arg_list = args->arg_list;
        struct arg_list *index = arg_list;
        if (debugMode) printf("[DEBUG -- CHECK: ITERATIVE] PARAMETERS: %d FROM call_resolve\n", numParams);
        while (index != NULL) // check that the params and args match up
        {
            arg_count++;
            if (arg_count > numParams)
            {
                printf("[ERROR] %s RECEIVED TOO MANY ARGUMENTS. EXPECTED %d. RECEIVED %d\n", call_id, numParams, arg_count);
                exit(1);
            }
            if (debugMode) printf("[DEBUG -- CHECK: TYPECHECK] PARAMETER FROM call_resolve\n");
            typeCheckPARAMS(index, call_id, arg_count);
            index = index->arg_list;
        }
        // check that arg_count == numParams
        if (arg_count < numParams)
        {
            printf("[ERROR] %s RECEIVED TOO FEW ARGUMENTS. EXPECTED %d. RECEIVED %d\n", call_id, numParams, arg_count);
            exit(1);
        }
    }
}

void args_resolve(struct args *args)
{
    argList_resolve(args->arg_list);
}

void argList_resolve(struct arg_list *arg_list)
{
    if (debugMode) printf("[DEBUG -- CHECK: ITERATIVE] FROM argList_resolve\n");
    struct arg_list *index = arg_list;
    while (index != NULL)
    {
        struct expression *currentExpr = index->expression;
        expression_resolve(currentExpr);
        index = index->arg_list;
    }
}

void typeCheckTERMS(struct term *term_LHS, struct term *term_RHS, char *symbol)
{
    struct factor *factor_LHS = term_LHS->factor;
    if (!factor_LHS)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] factor_LHS IN typeCheckTERMS\n");
        return;
    }
    struct factor *factor_RHS = term_RHS->factor;
    if (!factor_RHS)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] factor_RHS IN typeCheckTERMS\n");
        return;
    }
    if (debugMode) printf("[DEBUG -- CHECK: TYPECHECK] FACTORS FROM typeCheckTERMS\n");
    typeCheckFACTORS(factor_LHS, factor_RHS, symbol);
}

// Function that ensures two terms are both integers
void typeCheckFACTORS(struct factor *factor_LHS, struct factor *factor_RHS, char *symbol)
{
    struct call *call_LHS = factor_LHS->call;
    if (!call_LHS)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] call_LHS IN additiveExpression_resolve\n");
        return;
    }
    char *id_LHS = call_LHS->id;
    typeKind kind_LHS = scope_lookup_kind(head, id_LHS);

    // Type checking error handling for addop
    struct var *var_RHS = factor_RHS->var;
    if (var_RHS)
    {
        char *id_RHS = var_RHS->id;
        typeKind kind_RHS = scope_lookup_kind(head, id_RHS);
        if (kind_LHS != kind_RHS != INT_TYPE)
        {
            printf("[ERROR] OPERAND MISMATCH FOR %s BETWEEN %s AND %s\n", symbol, id_LHS, id_RHS); //, getAddopType(addop)
            exit(1);
        }
    }

    // Way to check if theres a value when the value exists by default
    if (!factor_RHS->call && !factor_RHS->expression && !factor_RHS->var)
    {
        if (kind_LHS != INT_TYPE)
        {
            printf("[ERROR] OPERAND MISMATCH FOR %s BETWEEN %s AND %d\n", symbol, id_LHS, factor_RHS->value); //, getAddopType(addop)
            exit(1);
        }
    }
}

void typeCheckARRAYS(struct expression *expression, char *id)
{
    // check that an array value doesn't get assigned to an var thats not an array
    struct simple_expression *simple_expression = expression->simple_expression;
    if (!simple_expression)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] simple_expression IN var_resolve\n");
        return;
    }
    struct additive_expression *additive_expression_left = simple_expression->additive_expression_left;
    if (!additive_expression_left)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] additive_expression_left IN var_resolve\n");
        return;
    }
    struct term *term = additive_expression_left->term;
    if (!term)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] term IN var_resolve\n");
        return;
    }
    struct factor *factor = term->factor;
    if (!factor)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] factor IN var_resolve\n");
        return;
    }
    if (!factor->call && !factor->expression && !factor->var)
    {
        bool hasArray = scope_lookup_hasArray(head, id);
        if (!hasArray)
        {
            printf("[ERROR] INCORRECT USE OF ARRAY INDEX SYNTAX FOR %s, ASSIGNING [%d]\n", id, factor->value);
            exit(1);
        }
    }
}

void typeCheckPARAMS(struct arg_list *index, char *call_id, int arg_count)
{
    struct expression *currentExpr = index->expression;
    if (!currentExpr)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] currentExpr IN call_resolve\n");
        return;
    }
    struct simple_expression *simple_expression = currentExpr->simple_expression;
    if (!simple_expression)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] simple_expression IN call_resolve\n");
        return;
    }
    struct additive_expression *additive_expression_left = simple_expression->additive_expression_left;
    if (!additive_expression_left)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] additive_expression_left IN call_resolve\n");
        return;
    }
    struct term *term = additive_expression_left->term;
    if (!term)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] term IN call_resolve\n");
        return;
    }
    struct factor *factor = term->factor;
    if (!factor)
    {
        if (debugMode) printf("[DEBUG - CHECK: UNDECLARED] factor IN call_resolve\n");
        return;
    }
    struct var *var = factor->var;
    if (var)
    {
        bool paramHasArr = getParamByValue(head, call_id, arg_count - 1);
        char *var_id = var->id;
        bool varHasArr = scope_lookup_hasArray(head, var_id);
        if (debugMode) {
            
            printf("[DEBUG - VALUE] FUNC: %s | PARAM: %s - INT", call_id, getParamNAMEByValue(head, call_id, arg_count-1));
            (paramHasArr) ? printf("[] | ") : printf(" | ");
            printf("ARG: %s - INT", var_id);
            (varHasArr) ? printf("[] |\n") : printf(" |\n");
        }
        if (paramHasArr != varHasArr)
        {
            if (paramHasArr)
                printf("[ERROR] INCORRECT FUNCTION CALL PARAMETERS. %s EXPECTED TO RECEIVE AN INT [], INSTEAD RECEIVED INT FROM %s\n", call_id, var_id);
            else
                printf("[ERROR] INCORRECT FUNCTION CALL PARAMETERS. %s EXPECTED TO RECEIVE AN INT, INSTEAD RECEIVED INT [] FROM %s\n", call_id, var_id);
            exit(1);
        }
    }
    if (!factor->call && !factor->expression && !factor->var)
    {
        bool paramHasArr = getParamByValue(head, call_id, arg_count - 1);
        int value = factor->value;
        if (debugMode) {
            printf("[DEBUG - VALUE] FUNC: %s | PARAM: %s - INT", call_id, getParamNAMEByValue(head, call_id, arg_count-1));
            (paramHasArr) ? printf("[] | ") : printf(" | ");
            printf("ARG: %d - INT |\n", value);
        }
        if (paramHasArr)
        {
            printf("[ERROR] INCORRECT FUNCTION CALL PARAMETERS. %s EXPECTED TO RECEIVE AN INT [], INSTEAD RECEIVED VALUE %d\n", call_id, value);
            exit(1);
        }
    }
}