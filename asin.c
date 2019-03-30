#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "asin_functions.h"

Token *currentToken;

int consume(int code)
{
    printf("[debug]: %s? => ", getCode(code));
    
    if (currentToken->code == code)
    {
        printf("consumat.\n");
        currentToken = currentToken->nxt;
        return 1;
    }
    
    printf("neconsumat (%s).\n", getCode(currentToken->code));
    
    return 0;
}

int expr()
{
    Token *startToken = currentToken;
    
    if (exprAssign())
        return 1;
        
    currentToken = startToken;
    return 0;
}

int exprPrimary()
{
    Token *startToken = currentToken;
    
    if (consume(ID))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                for (;;)
                {
                    if (consume(COMMA))
                    {
                        if (expr());
                        else break;
                    }    
                    else break;
                }
            }
            
            if (consume(RPAR))
                return 1;
        }
        
        return 1;
    }

    else if (consume(CT_INT))
        return 1;
        
    else if (consume(CT_REAL))
        return 1;
    
    else if (consume(CT_CHAR))
        return 1;
        
    else if (consume(CT_STRING))
        return 1;
        
    else if (consume(LPAR))
    {
        if (expr())
        {
            if (consume(RPAR))
                return 1;
        }
    }
    
    currentToken = startToken;
    return 0;
}

int exprUnary()
{
    Token *startToken = currentToken;

    if (consume(SUB))
    {
        if (exprUnary())
            return 1;
       
        else if (exprPostfix())
            return 1;
    }
    else if (consume(NOT))
    {
        if (exprUnary())
            return 1;
       
        else if (exprPostfix())
            return 1;
    }
    
    currentToken = startToken;
    return 0;
}

int exprAssign()
{
    if (exprUnary())
    {
        if (consume(ASSIGN))
        {
            if (exprAssign())
                return 1;
            
            else if (exprOr())
                return 1;
        }
    }
    
    return 0;
}

int arrayDecl()
{
    Token *startToken = currentToken;
    
    if (consume(LBRA))
    {
        if (expr())
        {
            if (consume(RBRA))
               return 1;
        }
        
        if (consume(RBRA))
            return 1;
    }
    
    currentToken = startToken;
    return 0;
}

int declStruct()
{
    Token *startToken = currentToken;
    
    printf("[debug]: declStruct %s?\n", getCode(currentToken->code));
    
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            if (consume(LACC))
            {
                for (;;)
                {
                    if (declVar());
                    else break;
                }
                
                if (consume(RACC))
                {
                    if (consume(SEMICOLON))
                        return 1;
                }
            }
        }
    }
    
    currentToken = startToken;
    return 0;
}

int typeBase()
{
    Token *startToken = currentToken;

    if (consume(INT))
        return 1;
    
    else if (consume(DOUBLE))
        return 1;
    
    else if (consume(CHAR))
        return 1;
    
    else if (consume(STRUCT))
    {
        if (consume(ID))
            return 1;
    }
    
    currentToken = startToken;
    return 0;
}

int typeName()
{
    Token *startToken = currentToken;

    if (typeBase())
    {
        if (arrayDecl());
        return 1;
    }

    currentToken = startToken;
    return 0;
}

int funcArg()
{
    Token *startToken = currentToken;
    
    if (typeBase())
    {
        if (consume(ID))
        {
            if (arrayDecl())
                return 1;
            
            return 1;
        }
    }
 
    currentToken = startToken;   
    return 0;
}               

int declFunc()
{
    Token *startToken = currentToken;
    
    printf("[debug]: declFunc %s?\n", getCode(currentToken->code));
    
    if (typeBase())
    {
        if (consume(MUL) || consume(VOID))
        {
            if (consume(ID))
            {
                if (consume(LPAR))
                {
                    if (funcArg())
                    {
                        for (;;)
                        {
                            if (consume(COMMA))
                            {
                                if (funcArg());
                                else break;
                            }
                            else break;
                        }
                    }

                    if (consume(RPAR))
                    {
                        if (stmCompound())
                            return 1;
                    }
                }
            }
        }
    }
    
    currentToken = startToken;
    return 0;
}

int declVar()
{
    Token *startToken = currentToken;

    if (typeBase())
    {
        if (consume(ID))
        {
            if (arrayDecl());
            for (;;)
            {
                if (consume(COMMA))
                {
                    if (consume(ID))
                    {
                        if (arrayDecl());
                    }
                    else break;
                }
                else break;
            }

            if (consume(SEMICOLON))
                return 1;
        }
    }

    currentToken = startToken;
    return 0;
}

int stmCompound()
{
    Token *startToken = currentToken;

    if (consume(LACC))
    {
        for (;;)
        {
            if (declVar());
            else if (stm());
            else break;
        }

        if (consume(RACC))
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int stm()
{
    Token *startToken = currentToken;

    if (stmCompound())
        return 1;

    else if (consume(IF))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                    {
                        if (consume(ELSE))
                        {
                            if (stm())
                                return 1;
                        }

                        return 1;
                    }
                }
            }
        }
    }
    else if (consume(WHILE))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                        return 1;
                }
            }
        }
    }
    else if (consume(FOR))
    {
        if (consume(LPAR))
        {
            if (expr());
            if (consume(SEMICOLON))
            {
                if (expr());
                if (consume(SEMICOLON))
                {
                    if (expr());
                    if (consume(RPAR))
                    {
                        if (stm())
                            return 1;
                    }
                }
            }
        }
    }

    else if (consume(BREAK))
    {
        if (consume(SEMICOLON))
            return 1;
    }

    else if (consume(RETURN))
    {
        if (expr());
        if (consume(SEMICOLON))
            return 1;
    }
    else if (expr())
    {
        if (consume(SEMICOLON))
            return 1;
    }
    else if (consume(SEMICOLON))
        return 1;

    currentToken = startToken;
    return 0;
}

int unit()
{
    Token *startToken = currentToken;
    
    for (;;)
    {
        if (declStruct());
        else if (declFunc());
        else if (declVar());
        else break;
    }
    
    if (consume(END))
        return 1;
    
    currentToken = startToken;
    return 0;
}

int exprOrPrim()
{
    Token *startToken = currentToken;

    if (consume(OR))
    {
        if (exprAnd())
        {
            if (exprOrPrim())
                return 1;
        }
    }
    
    currentToken = startToken;
    return 1;
}

int exprOr()
{
    Token *startToken = currentToken;
    
    if (exprAnd())
    {
        if (exprOrPrim())
            return 1;
    }
    
    currentToken = startToken;
    return 0;
}

int exprAnd()
{
    Token *startToken = currentToken;

    if (exprEq())
    {
        if (exprAndPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprAndPrim()
{
    Token *startToken = currentToken;

    if (consume(AND))
    {
        if (exprEq())
        {
            if (exprAndPrim())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprEq()
{
    Token *startToken = currentToken;

    if (exprRel())
    {
        if (exprEqPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprEqPrim()
{
    Token *startToken = currentToken;

    if (consume(EQUAL) || consume(NOTEQ))
    {
        if (exprRel())
        {
            if (exprRel())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprRel()
{
    Token *startToken = currentToken;

    if (exprAdd())
    {
        if (exprRelPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprRelPrim()
{
    Token *startToken = currentToken;

    if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATREQ))
    {
        if (exprAdd())
        {
            if (exprRelPrim())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprAdd()
{
    Token *startToken = currentToken;

    if (exprMul())
    {
        if (exprAddPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprAddPrim()
{
    Token *startToken = currentToken;

    if (consume(ADD) || consume(SUB))
    {
        if (exprMul())
        {
            if (exprAndPrim())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprMul()
{
    Token *startToken = currentToken;

    if (exprCast())
    {
        if (exprMulPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprMulPrim()
{
    Token *startToken = currentToken;

    if (consume(MUL) || consume(DIV))
    {
        if (exprCast())
        {
            if (exprMulPrim())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprPostfix()
{
    Token *startToken = currentToken;

    if (exprPrimary())
    {
        if (exprPostfixPrim())
            return 1;
    }

    currentToken = startToken;
    return 0;
}

int exprPostfixPrim()
{
    Token *startToken = currentToken;

    if (consume(LBRA))
    {
        if (expr())
        {
            if (consume(RBRA))
            {
                if (exprPostfixPrim())
                    return 1;
            }
        }
    }
    else if (consume(DOT))
    {
        if (consume(ID))
        {
            if (exprPostfixPrim())
                return 1;
        }
    }

    currentToken = startToken;
    return 1;
}

int exprCast()
{
    Token *startToken = currentToken;

    if (consume(LPAR))
    {
        if (typeName())
        {
            if (consume(RPAR))
            {
                if (exprCast())
                    return 1;
            }
        }
    }

    else if (exprUnary())
        return 1;

    currentToken = startToken;
    return 0;
}

int main()
{
    currentToken = h;
    
    if (unit())
        fprintf(stdout, "syntax is OK.\n");
    
    else
        fprintf(stdout, "syntax error.\n");
    
    return 0;
}