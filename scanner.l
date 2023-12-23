%{ 
    #include "parser.h" //#include "token.h"
    int yyerror(const char *s);
%} 

DIGIT [0-9] 
LETTER [a-z|A-Z] 

%% 
"else"        { return TOKEN_ELSE; } 
"if"          { return TOKEN_IF; }
"int"         { return TOKEN_INT; }
"return"      { return TOKEN_RETURN; }
"void"        { return TOKEN_VOID; }
"while"       { return TOKEN_WHILE; }
"+"          { return TOKEN_ADD; }  
"-"          { return TOKEN_MINUS; } 
"*"          { return TOKEN_MUL; } 
"/"          { return TOKEN_DIV; } 
"<="         { return TOKEN_LESS_EQ; }
"<"          { return TOKEN_LESS; }
">="         { return TOKEN_MORE_EQ; }
">"          { return TOKEN_MORE; }
"=="         { return TOKEN_EQ_COMP; }
"="          { return TOKEN_EQ; }
";"          { return TOKEN_SEMI; }
","          { return TOKEN_COMMA; }
"("          { return TOKEN_PAREN_OP; } 
")"          { return TOKEN_PAREN_CL; } 
"{"          { return TOKEN_CURLY_OP; }
"}"          { return TOKEN_CURLY_CL; }
"["          { return TOKEN_SQUARE_OP; }
"]"          { return TOKEN_SQUARE_CL; }
{LETTER}+   {yylval.id = strdup(yytext); return TOKEN_ID; }
{DIGIT}+    {yylval.num = atoi(yytext); return TOKEN_NUM; }    
""  {}
(" "|\t|\n|\r) /* Ignore these */
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]       { /* Ignore single/multiline comments https://stackoverflow.com/questions/25395251/detecting-and-skipping-line-comments-with-flex */ }
.           { return TOKEN_ERROR; }

%% 

int yywrap() { return 1; }