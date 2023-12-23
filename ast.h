#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// typedef enum for the different types of declarations, type-specifiers, relops, addops, & mulops
typedef enum
{
    VAR,
    FUNC,
} decl_type;

typedef enum
{
    INT_TYPE,
    VOID_TYPE,
} typeKind;

typedef enum
{
    LESS_EQ,
    LESS,
    MORE_EQ,
    MORE,
    EQ_COMP,
    EQ,
} relop_type;

typedef enum
{
    ADD,
    MINUS,
} addop_type;

typedef enum
{
    MUL,
    DIV,
} mulop_type;

// Structs for the AST
struct declaration
{
    decl_type declType;
    struct var_declaration *var_declaration;
    struct fun_declaration *fun_declaration;
    struct declaration *next;
};

struct var_declaration
{
    struct type_specifier *type_specifier;
    char *id;
    int value;
};

struct type_specifier
{
    typeKind kind;
};

struct fun_declaration
{
    struct type_specifier *type_specifier;
    char *id;
    struct param *param;
    struct compound_stmt *compound_stmt;
};

struct param
{
    struct type_specifier *type_specifier;
    char *id;
    bool hasArray;
    struct param *next;
};

struct compound_stmt
{
    struct local_declarations *local_declarations;
    struct statement_list *statement_list;
};

struct local_declarations
{
    struct var_declaration *var_declaration;
    struct local_declarations *next;
};

struct statement_list
{
    struct statement *statement;
    struct statement_list *next;
};

struct statement
{
    struct expression_stmt *expression_stmt;
    struct compound_stmt *compound_stmt;
    struct selection_stmt *selection_stmt;
    struct iteration_stmt *iteration_stmt;
    struct return_stmt *return_stmt;
};

struct expression_stmt
{
    struct expression *expression;
};

struct selection_stmt
{
    struct expression *expression;
    struct statement *then_statement;
    struct statement *else_statement;
};

struct iteration_stmt
{
    struct expression *expression;
    struct statement *statement;
};

struct return_stmt
{
    struct expression *expression;
};

struct expression
{
    struct var *var;
    struct expression *expression;
    struct simple_expression *simple_expression;
};

struct var
{
    char *id;
    struct expression *expression;
};

struct simple_expression
{
    struct additive_expression *additive_expression_left;
    struct relop *relop;
    struct additive_expression *additive_expression_right;
};

struct relop
{
    relop_type relopType;
};

struct additive_expression
{
    struct additive_expression *additive_expression;
    struct addop *addop;
    struct term *term;
};

struct addop
{
    addop_type addopType;
};

struct term
{
    struct term *term;
    struct mulop *mulop;
    struct factor *factor;
};

struct mulop
{
    mulop_type mulopType;
};

struct factor
{
    struct expression *expression;
    struct var *var;
    struct call *call;
    int value;
};

struct call
{
    char *id;
    struct args *args;
};

struct args
{
    struct arg_list *arg_list;
};

struct arg_list
{
    struct arg_list *arg_list;
    struct expression *expression;
};

// Constructors for the different structs for the AST
struct declaration *createDecl(decl_type declType, struct var_declaration *var_declaration, struct fun_declaration *fun_declaration, struct declaration *next)
{
    struct declaration *newDecl = malloc(sizeof *newDecl);
    newDecl->declType = declType;
    newDecl->var_declaration = var_declaration;
    newDecl->fun_declaration = fun_declaration;
    newDecl->next = next;
    return newDecl;
}

struct var_declaration *createVarDecl(struct type_specifier *type_specifier, char *id, int value)
{
    struct var_declaration *newVarDecl = malloc(sizeof *newVarDecl);
    newVarDecl->type_specifier = type_specifier;
    newVarDecl->id = id;
    newVarDecl->value = value;
    return newVarDecl;
}

struct type_specifier *createType(typeKind kind)
{
    struct type_specifier *newType = malloc(sizeof *newType);
    newType->kind = kind;
    return newType;
}

struct fun_declaration *createFuncDecl(struct type_specifier *type_specifier, char *id, struct param *param, struct compound_stmt *compound_stmt)
{
    struct fun_declaration *newFuncDecl = malloc(sizeof *newFuncDecl);
    newFuncDecl->type_specifier = type_specifier;
    newFuncDecl->id = id;
    newFuncDecl->param = param;
    newFuncDecl->compound_stmt = compound_stmt;
    return newFuncDecl;
}

struct param *createParam(struct type_specifier *type_specifier, char *id, bool hasArray, struct param *next)
{
    struct param *newParam = malloc(sizeof *newParam);
    newParam->type_specifier = type_specifier;
    newParam->id = id;
    newParam->hasArray = hasArray;
    newParam->next = next;
    return newParam;
}

struct compound_stmt *createCompoundStmt(struct local_declarations *local_declarations, struct statement_list *statement_list)
{
    struct compound_stmt *newCompoundStmt = malloc(sizeof *newCompoundStmt);
    newCompoundStmt->local_declarations = local_declarations;
    newCompoundStmt->statement_list = statement_list;
    return newCompoundStmt;
}

struct local_declarations *createLocalDecl(struct var_declaration *var_declaration, struct local_declarations *next)
{
    struct local_declarations *newLocalDecl = malloc(sizeof *newLocalDecl);
    newLocalDecl->var_declaration = var_declaration;
    newLocalDecl->next = next;
    return newLocalDecl;
}

struct statement_list *createStatementList(struct statement *statement, struct statement_list *next)
{
    struct statement_list *newStatmentList = malloc(sizeof *newStatmentList);
    newStatmentList->statement = statement;
    newStatmentList->next = next;
    return newStatmentList;
}

struct statement *createStatement(struct expression_stmt *expression_stmt, struct compound_stmt *compound_stmt, struct selection_stmt *selection_stmt, struct iteration_stmt *iteration_stmt, struct return_stmt *return_stmt)
{
    struct statement *newStatement = malloc(sizeof *newStatement);
    newStatement->expression_stmt = expression_stmt;
    newStatement->compound_stmt = compound_stmt;
    newStatement->selection_stmt = selection_stmt;
    newStatement->iteration_stmt = iteration_stmt;
    newStatement->return_stmt = return_stmt;
    return newStatement;
}

struct expression_stmt *createExpressionStatement(struct expression *expression)
{
    struct expression_stmt *newExprStmt = malloc(sizeof *newExprStmt);
    newExprStmt->expression = expression;
    return newExprStmt;
}

struct selection_stmt *createSelectionStmt(struct expression *expression, struct statement *then_statement, struct statement *else_statement)
{
    struct selection_stmt *newSelectionStmt = malloc(sizeof *newSelectionStmt);
    newSelectionStmt->expression = expression;
    newSelectionStmt->then_statement = then_statement;
    newSelectionStmt->else_statement = else_statement;
    return newSelectionStmt;
}

struct iteration_stmt *createIterationStmt(struct expression *expression, struct statement *statement)
{
    struct iteration_stmt *newIterStmt = malloc(sizeof *newIterStmt);
    newIterStmt->expression = expression;
    newIterStmt->statement = statement;
    return newIterStmt;
}

struct return_stmt *createReturnStmt(struct expression *expression)
{
    struct return_stmt *newReturnStmt = malloc(sizeof *newReturnStmt);
    newReturnStmt->expression = expression;
    return newReturnStmt;
}

struct expression *createExpr(struct var *var, struct expression *expression, struct simple_expression *simple_expression)
{
    struct expression *newExpr = malloc(sizeof *newExpr);
    newExpr->var = var;
    newExpr->expression = expression;
    newExpr->simple_expression = simple_expression;
    return newExpr;
}

struct var *createVar(char *id, struct expression *expression)
{
    struct var *newVar = malloc(sizeof *newVar);
    newVar->id = id;
    newVar->expression = expression;
    return newVar;
}

struct simple_expression *createSimpleExpr(struct additive_expression *additive_expression_left, struct relop *relop, struct additive_expression *additive_expression_right)
{
    struct simple_expression *newSimp = malloc(sizeof *newSimp);
    newSimp->additive_expression_left = additive_expression_left;
    newSimp->relop = relop;
    newSimp->additive_expression_right = additive_expression_right;
    return newSimp;
}

struct relop *createRelop(relop_type relopType)
{
    struct relop *newRelop = malloc(sizeof *newRelop);
    newRelop->relopType = relopType;
    return newRelop;
}

struct additive_expression *createAdditiveExpression(struct additive_expression *additive_expression, struct addop *addop, struct term *term)
{
    struct additive_expression *newAddExpr = malloc(sizeof *newAddExpr);
    newAddExpr->additive_expression = additive_expression;
    newAddExpr->addop = addop;
    newAddExpr->term = term;
    return newAddExpr;
}

struct addop *createAddop(addop_type addopType)
{
    struct addop *newAddop = malloc(sizeof *newAddop);
    newAddop->addopType = addopType;
    return newAddop;
}

struct term *createTerm(struct term *term, struct mulop *mulop, struct factor *factor)
{
    struct term *newTerm = malloc(sizeof *newTerm);
    newTerm->term = term;
    newTerm->mulop = mulop;
    newTerm->factor = factor;
    return newTerm;
}

struct mulop *createMulop(mulop_type mulopType)
{
    struct mulop *newMulop = malloc(sizeof *newMulop);
    newMulop->mulopType = mulopType;
    return newMulop;
}

struct factor *createFactor(struct expression *expression, struct var *var, struct call *call, int value)
{
    struct factor *newFactor = malloc(sizeof *newFactor);
    newFactor->expression = expression;
    newFactor->var = var;
    newFactor->call = call;
    newFactor->value = value;
    return newFactor;
}

struct call *createCall(char *id, struct args *args)
{
    struct call *newCall = malloc(sizeof *newCall);
    newCall->id = id;
    newCall->args = args;
    return newCall;
}

struct args *createArgs(struct arg_list *arg_list)
{
    struct args *newArgs = malloc(sizeof *newArgs);
    newArgs->arg_list = arg_list;
    return newArgs;
}

struct arg_list *createArgList(struct arg_list *arg_list, struct expression *expression)
{
    struct arg_list *newArgList = malloc(sizeof *newArgList);
    newArgList->arg_list = arg_list;
    newArgList->expression = expression;
    return newArgList;
}

// Functions for printing the AST - typedef enum
char *getFuncType(struct declaration *decl)
{
    switch (decl->declType)
    {
    case VAR:
        return "variable";
    case FUNC:
        return "function";
    default:
        break;
    }
}

char *getType(struct type_specifier *type)
{
    switch (type->kind)
    {
    case INT_TYPE:
        return "int";
    case VOID_TYPE:
        return "void";
    default:
        break;
    }
}

char *getRelopType(struct relop *Relop)
{
    switch (Relop->relopType)
    {
    case LESS_EQ:
        return "<=";
    case LESS:
        return "<";
    case MORE_EQ:
        return ">=";
    case MORE:
        return ">";
    case EQ_COMP:
        return "==";
    case EQ:
        return "=";
    default:
        break;
    }
}

char *getAddopType(struct addop *Addop)
{
    switch (Addop->addopType)
    {
    case ADD:
        return "+";
    case MINUS:
        return "-";
    default:
        break;
    }
}

char *getMulopType(struct mulop *Mulop)
{
    switch (Mulop->mulopType)
    {
    case MUL:
        return "*";
    case DIV:
        return "/";
    default:
        break;
    }
}

// Tabs to make the AST into a tree
bool start = true;
int tabCount = 0;
void printTab(void)
{
    for (int i = 0; i < tabCount; i++)
        printf("  ");
}

// Odd spot for declarations
void printDecl(struct declaration *decl);
void printVarDecl(struct var_declaration *varDecl);
void printFunDecl(struct fun_declaration *funcDecl);
void printParam(struct param *Param);
void printCompoundStmt(struct compound_stmt *compound_stmt);
void printLocalDecl(struct local_declarations *local_declarations);
void printStatementList(struct statement_list *statement_list);
void printStatement(struct statement *statement);
void printExpressionStatement(struct expression_stmt *expression_stmt);
void printSelectionStmt(struct selection_stmt *selection_stmt);
void printIterationStmt(struct iteration_stmt *iteration_stmt);
void printReturnStmt(struct return_stmt *return_stmt);
void printExpression(struct expression *expression);
void printVar(struct var *var);
void printSimpleExpression(struct simple_expression *simple_expression);
void printAdditiveExpression(struct additive_expression *additive_expression);
void printTerm(struct term *term);
void printFactor(struct factor *factor);
void printCall(struct call *call);
void printArgs(struct args *args);
void printArgList(struct arg_list *arg_list);

// Functions for printing the AST - Printing the components
void printDecl(struct declaration *decl)
{
    if (!decl)
        return;
    if (start)
    {
        start = false;
        printf("program\n");
        tabCount++;
    }

    if (decl->declType == VAR)
    {
        printVarDecl(decl->var_declaration);
    }
    if (decl->declType == FUNC)
    {
        printTab();
        printf("func\n");
        tabCount++;
        printFunDecl(decl->fun_declaration);
        printTab();
        printf("block\n");
        struct compound_stmt *cmpStmt = decl->fun_declaration->compound_stmt;
        if (cmpStmt)
        {
            printCompoundStmt(cmpStmt);
        }
        tabCount--;
    }
    // tabCount = 1;
    printDecl(decl->next);
}

void printVarDecl(struct var_declaration *varDecl)
{
    printTab();
    printf("var_decl\n");
    char *varType = getType(varDecl->type_specifier);
    char *id = varDecl->id;
    int value = varDecl->value;
    tabCount++;
    printTab();
    printf("type [%s]\n", varType);
    printTab();
    printf("new_id [%s]", id);
    if (value)
        printf("[%d]", value);
    printf("\n");
    tabCount--;
}

void printFunDecl(struct fun_declaration *funcDecl)
{
    char *funcType = getType(funcDecl->type_specifier);
    char *id = funcDecl->id;
    printTab();
    printf("type [%s]\n", funcType);
    printTab();
    printf("new_id [%s]\n", id);
    printTab();
    printf("parameters\n");
    tabCount++;
    printParam(funcDecl->param);
}

void printParam(struct param *Param)
{
    if (!Param)
    {
        tabCount--;
        return;
    }
    char *paramType = getType(Param->type_specifier);
    char *id = Param->id;
    bool array = Param->hasArray;
    if (!(strcmp(paramType, "void")) && (id == NULL))
    { // case when only parameter is void
        tabCount--;
        return;
    }
    printTab();
    printf("var_param\n");
    tabCount++;
    printTab();
    printf("type [%s]\n", paramType);
    printTab();
    printf("new_id [%s]", id);
    if (array)
        printf("[]");
    printf("\n");
    tabCount--;
    printParam(Param->next);
}

void printCompoundStmt(struct compound_stmt *compound_stmt)
{
    tabCount++;
    printTab();
    printf("declarations\n");
    if (compound_stmt->local_declarations)
        printLocalDecl(compound_stmt->local_declarations);
    printTab();
    printf("statements\n");
    if (compound_stmt->statement_list)
        printStatementList(compound_stmt->statement_list);
    tabCount--;
}

void printLocalDecl(struct local_declarations *local_declarations)
{
    struct local_declarations *currentLD = local_declarations;
    while (currentLD != NULL)
    {
        struct var_declaration *currentVD = currentLD->var_declaration;
        printVarDecl(currentVD);
        currentLD = currentLD->next;
    }
}

void printStatementList(struct statement_list *statement_list)
{
    struct statement_list *index = statement_list;

    while (index != NULL)
    {
        struct statement *currentStatement = index->statement;
        printStatement(currentStatement);
        index = index->next;
    }
}

void printStatement(struct statement *statement)
{
    tabCount++;
    struct expression_stmt *expression_stmt = statement->expression_stmt;
    struct compound_stmt *compound_stmt = statement->compound_stmt;
    struct selection_stmt *selection_stmt = statement->selection_stmt;
    struct iteration_stmt *iteration_stmt = statement->iteration_stmt;
    struct return_stmt *return_stmt = statement->return_stmt;

    if (expression_stmt)
        printExpressionStatement(expression_stmt);
    if (compound_stmt)
        printCompoundStmt(compound_stmt);
    if (selection_stmt)
        printSelectionStmt(selection_stmt);
    if (iteration_stmt)
        printIterationStmt(iteration_stmt);
    if (return_stmt)
        printReturnStmt(return_stmt);
    tabCount--;
}

void printExpressionStatement(struct expression_stmt *expression_stmt)
{
    struct expression *expression = expression_stmt->expression;
    printTab();
    printf("exprstmt\n");
    if(expression)
        printExpression(expression);
}

void printSelectionStmt(struct selection_stmt *selection_stmt)
{
    struct expression *expression = selection_stmt->expression;
    struct statement *then_statement = selection_stmt->then_statement;
    struct statement *else_statement = selection_stmt->else_statement;

    if (else_statement)
    {
        printTab();
        printf("if_else\n");
        if(expression)
            printExpression(expression);
        printStatement(then_statement);
        printStatement(else_statement);
        return;
    }
    printTab();
    printf("if\n");
    if(expression)
        printExpression(expression);
    printStatement(then_statement);
}

void printIterationStmt(struct iteration_stmt *iteration_stmt)
{
    struct expression *expression = iteration_stmt->expression;
    struct statement *statement = iteration_stmt->statement;
    printTab();
    printf("while (n");
    if(expression)
        printExpression(expression);
    printf(")\n");
    printStatement(statement);
}

void printReturnStmt(struct return_stmt *return_stmt)
{
    struct expression *expression = return_stmt->expression;
    printTab();
    printf("return\n");
    if (expression)
        printExpression(expression);
}

void printExpression(struct expression *expression)
{
    struct var *var = expression->var;
    struct expression *nextExp = expression->expression;
    struct simple_expression *simple_expression = expression->simple_expression;
    if (var)
    {
        printVar(var);
        printTab();
        printf("=\n");
        tabCount++;
        printExpression(expression);
        tabCount--;
    }
    if (simple_expression)
    {
        tabCount++;
        printSimpleExpression(simple_expression);
        tabCount--;
    }
}

void printVar(struct var *var)
{
    tabCount++;
    printTab();
    char *id = var->id;
    struct expression *expression = var->expression;
    printf("var [%s]\n", id);
    if (expression)
        printExpression(expression);
    tabCount--;
}

void printSimpleExpression(struct simple_expression *simple_expression)
{
    struct additive_expression *additive_expression_left = simple_expression->additive_expression_left;
    struct relop *relop = simple_expression->relop;
    struct additive_expression *additive_expression_right = simple_expression->additive_expression_right;
    if (relop)
    {
        char *relopType = getRelopType(relop);
        printTab();
        printf("%s\n", relopType);
        printAdditiveExpression(additive_expression_left);
        // printf("HERE: %d\n", additive_expression_right->term->factor->value);
        printAdditiveExpression(additive_expression_right);
        return;
    }
    printAdditiveExpression(additive_expression_left);
}

void printAdditiveExpression(struct additive_expression *additive_expression)
{
    struct additive_expression *additive_expression_here = additive_expression->additive_expression;
    struct addop *addop = additive_expression->addop;
    struct term *term = additive_expression->term;
    if (addop)
    {
        printAdditiveExpression(additive_expression_here);
        char *addopType = getAddopType(addop);
        tabCount++;
        printTab();
        printf("%s\n", addopType);
        printTerm(term);
        tabCount--;
        return;
    }

    printTerm(term);
    return;
}

void printTerm(struct term *term)
{
    struct term *term_here = term->term;
    struct mulop *mulop = term->mulop;
    struct factor *factor = term->factor;
    if (mulop)
    {
        printTerm(term_here);
        tabCount++;
        printTab();
        char *mulopType = getMulopType(mulop);
        printf("%s\n", mulopType);
        printFactor(factor);
        tabCount--;
        return;
    }
    printFactor(factor);
}

void printFactor(struct factor *factor)
{
    struct expression *expression = factor->expression;
    struct var *var = factor->var;
    struct call *call = factor->call;
    int value = factor->value;

    if (expression)
    {
        printExpression(expression);
        return;
    }

    if (var)
    {
        printVar(var);
        return;
    }

    if (call)
    {
        printCall(call);
        return;
    }
    tabCount++;
    printTab();
    printf("int [%d]\n", value);
    tabCount--;
}

void printCall(struct call *call)
{
    char *id = call->id;
    struct args *args = call->args;
    tabCount++;
    printTab();
    printf("funccall\n");
    tabCount++;
    printTab();
    printf("var [%s]\n", id);
    printTab();
    printf("arguments\n");
    if (args)
    {
        tabCount -= 2;
        printArgs(args);
        tabCount += 2;
    }
    tabCount -= 2;
}

void printArgs(struct args *args)
{
    struct arg_list *arg_list = args->arg_list;
    printArgList(arg_list);
}

void printArgList(struct arg_list *arg_list)
{
    struct arg_list *index = arg_list;
    tabCount++;
    while (index != NULL)
    {
        struct expression *currentExpr = index->expression;
        printExpression(currentExpr);
        index = index->arg_list;
    }
    tabCount--;
}
