#ifndef _ASIN_INCLUDE
	#define _ASIN_INCLUDE
	
	#include "asyn_functions.h"

	int consume(int code)
	{
		#if defined _DEBUG_MODE
	    	printf("[debug]: %s? => ", getTokenName(code));
	    #endif

	    if (currentToken->code == code)
	    {
	    	#if defined _DEBUG_MODE
	        	printf("consumat.\n");
	        #endif

	        consumedToken = currentToken;
	        currentToken = currentToken->nxt;
	        return 1;
	    }
	    
	    #if defined _DEBUG_MODE
	    	printf("neconsumat (%s).\n", getTokenName(currentToken->code));
	    #endif

	    return 0;
	}

	int expr()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: expr %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprAssign())
	        return 1;
	        
	    currentToken = startToken;
	    return 0;
	}

	int exprPrimary()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprPrimary %s?\n", getTokenName(currentToken->code));
	    #endif

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
	                        if (expr())
	                        	continue;

	                        else
	                        	tkerr(currentToken, "Missing expression after ','.");
	                    }    
	                    break;
	                }
	            }
	            
	            if (consume(RPAR))
	                return 1;

	            else
	            	tkerr(currentToken, "Missing ')' after ID declaration.");
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

	            else
	            	tkerr(currentToken, "Missing ) after expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing expression.");
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	int exprUnary()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprUnary %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(SUB) || consume(NOT))
	    {
	        if (exprUnary())
	            return 1;
	    }

	    else if (exprPostfix())
	    	return 1;
	    
	    currentToken = startToken;
	    return 0;
	}

	int exprAssign()
	{
		Token *startToken = currentToken;

		#if defined _DEBUG_MODE
			printf("[debug]: exprAssign %s?\n", getTokenName(currentToken->code));
		#endif

	    if (exprUnary())
	    {
	        if (consume(ASSIGN))
	        {
	            if (exprAssign())
	                return 1;

	            else
	            	tkerr(currentToken, "Missing assign expression.");
	        }
	    }
	    
	    currentToken = startToken;
	    
	    if (exprOr())
	    	return 1;
	    
	    currentToken = startToken;
	    return 0;
	}

	int arrayDecl(Type *ret)
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: arrayDecl %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(LBRA))
	    {
	        expr();
	        ret->nElements = 0;
	        
	        if (consume(RBRA))
	            return 1;

	        else
	        	tkerr(currentToken, "Missing '[' at array declaration.");
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	int declStruct()
	{
	    Token *startToken = currentToken;
	    Token *tkName;
	    
	    #if defined _DEBUG_MODE
	    	printf("[debug]: declStruct %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(STRUCT))
	    {
	        if (consume(ID))
	        {
	        	tkName = consumedToken;
	            if (consume(LACC))
	            {
		        	if (findSymbol(&symbols, tkName->text))
		        		tkerr(currentToken, "symbol redefinition: %s", tkName->text);
		        		
		        	currentStruct = addSymbol(&symbols, tkName->text, CLS_STRUCT);
		        	initSymbols(&currentStruct->members);
	            		
	                for (;;)
	                {
	                    if (declVar())
	                    	continue;

	                    else break;
	                }
	                
	                if (consume(RACC))
	                {
	                    if (consume(SEMICOLON))
	                    {
	                    	currentStruct = NULL;
	                        return 1;
	                    }
	                }
	            }
	        }
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	void addVar(Token *tkName, Type *t)
	{
		Symbol *s;
		
		if (currentStruct)
		{
			if (findSymbol(&currentStruct->members, tkName->text))
				tkerr(currentToken, "symbol redefinition: %s", tkName->text);
				
			s = addSymbol(&currentStruct->members, tkName->text, CLS_VAR);
		}
		else if (currentFunc)
		{
			s = findSymbol(&symbols, tkName->text);
			if (s && s->depth == currentDepth)
				tkerr(currentToken, "symbol redefinition: %s", tkName->text);
				
			s = addSymbol(&symbols, tkName->text, CLS_VAR);
			s->mem = MEM_LOCAL;
		}
		else
		{
			if (findSymbol(&symbols, tkName->text))
				tkerr(currentToken, "symbol redefinition: %s", tkName->text);
				
			s = addSymbol(&symbols, tkName->text, CLS_VAR);
			s->mem = MEM_GLOBAL;
		}
		
		s->type = *t;
	}

	int typeBase(Type *ret)
	{
	    Token *startToken = currentToken;
	    Token *tkName;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: typeBase %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(INT))
	    {
	    	ret->typeBase = TB_INT;
	        return 1;
	    }
	    else if (consume(DOUBLE))
	    {
	    	ret->typeBase = TB_DOUBLE;
	        return 1;
	    }
	    else if (consume(CHAR))
	    {
	    	ret->typeBase = TB_CHAR;
	        return 1;
	    }
	    else if (consume(STRUCT))
	    {
	        if (consume(ID))
	        {
	        	tkName = consumedToken;
	        	Symbol *s = findSymbol(&symbols, tkName->text);
	        	
	        	if (s == NULL)
	        		tkerr(currentToken, "undefined symbol: %s", tkName->text);
	    		
	    		if (s->cls != CLS_STRUCT)
	    			tkerr(currentToken, "%s is not a struct", tkName->text);
	    			
				ret->typeBase = TB_STRUCT;
				ret->s = s;
	            return 1;
			}
	        else
	        	tkerr(currentToken, "Missing ID after STRUCT.");
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	int typeName(Type *ret)
	{
	    if (typeBase(ret))
	    {
	        if (arrayDecl(ret))
	        	;
	        
	        else
	        	ret->nElements = -1;
	        	
	        return 1;
	    }
	    return 0;
	}

	int funcArg()
	{
	    Token *startToken = currentToken;
	    Token *tkName;
	    Type t;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: funcArg %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (typeBase(&t))
	    {
	        if (consume(ID))
	        {
	        	tkName = consumedToken;
	            if (arrayDecl(&t))
	            	;
	            	
	            else
	            	t.nElements = -1;
	            	
	            Symbol *s = addSymbol(&symbols, tkName->text, CLS_VAR);
	            s->mem = MEM_ARG;
	            s->type = t;

	            s = addSymbol(&currentFunc->args, tkName->text, CLS_VAR);
	            s->mem = MEM_ARG;
	            s->type = t;
	            	
	            return 1;
	        }
	        else
	        	tkerr(currentToken, "Missing ID for function argument.");
	    }
	 
	    currentToken = startToken;   
	    return 0;
	}

	int declFunc()
	{
	    Token *startToken = currentToken;
	    Token *tkName;
	    Type t;
	    
	    #if defined _DEBUG_MODE
	    	printf("[debug]: declFunc %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (typeBase(&t))
	    {
	    	if (consume(MUL))
	    		t.nElements = 0;
	    	
	    	else
	    		t.nElements = -1;
	       	
	       	goto DECLFUNC;
	    }

	    else if (consume(VOID))
	    {
	    	t.typeBase = TB_VOID;
	    	goto DECLFUNC;
		}
		
	    DECLFUNC:
	    if (consume(ID))
	    {
	    	tkName = consumedToken;
	        if (consume(LPAR))
	        {
	        	if (findSymbol(&symbols, tkName->text))
	        		tkerr(currentToken, "symbol redefinition: %s", tkName->text);
	        		
	        	currentFunc = addSymbol(&symbols, tkName->text, CLS_FUNC);
	        	
	        	initSymbols(&currentFunc->args);
	        	
	        	currentFunc->type = t;
	        	currentDepth ++;
	        	
	            if (funcArg())
	            {
	                for (;;)
	                {
	                    if (consume(COMMA))
	                    {
	                        if (funcArg())
	                        	continue;

	                        else
	                        	tkerr(currentToken, "Missing function arguments at function declaration.");
	                    }
	                    else break;
	                }
	            }

	            if (consume(RPAR))
	            {
	            	currentDepth --;
	            	
	                if (stmCompound())
	                {
	                	deleteSymbolsAfter(&symbols, currentFunc);
	                	currentFunc = NULL;
	                    return 1;
					}
	                else
	                	tkerr(currentToken, "Missing statement after ')'.");
	            }

	            else
	                tkerr(currentToken, "Missing ')' at function declaration.");
	        }
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	int declVar()
	{
	    Token *startToken = currentToken;
	    Token *tkName;
		Type t;

		#if defined _DEBUG_MODE
	    	printf("[debug]: declVar %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (typeBase(&t))
	    {
	        if (consume(ID))
	        {
	        	tkName = consumedToken;
	        	
	            if (arrayDecl(&t))
	            	;
	            	
	            else
	            	t.nElements = -1;
	            	
	            addVar(tkName, &t);
	            	
	            for (;;)
	            {
	                if (consume(COMMA))
	                {
	                    if (consume(ID))
	                    {
	                    	tkName = consumedToken;
	                    	
							if (arrayDecl(&t))
								;
							
							else
								t.nElements = -1;
						
							addVar(tkName, &t);
						}
	                    
	                    else 
	                    	tkerr(currentToken, "Missing ID.");
	                }
	                else break;
	            }

	            if (consume(SEMICOLON))
	                return 1;

	            else
	            	tkerr(currentToken, "Missing ;.");
	        }
	    }

	    currentToken = startToken;
	    return 0;
	}

	int stmCompound()
	{
	    Token *startToken = currentToken;
	    Symbol *start = symbols.end[-1];

	    #if defined _DEBUG_MODE
	    	printf("[debug]: stmCompound %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(LACC))
	    {
	    	currentDepth ++;
	    	
	        for (;;)
	        {
	            if (declVar())
	            	continue;

	            else if (stm())
	            	continue;

	            else break;
	        }

	        if (consume(RACC))
	        {
	        	currentDepth --;
	        	deleteSymbolsAfter(&symbols, start);
	            return 1;
			}
	        else
	        	tkerr(currentToken, "Missing }.");
	    }

	    currentToken = startToken;
	    return 0;
	}

	int stm()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: stm %s?\n", getTokenName(currentToken->code));
	    #endif

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

	                            else
	                            	tkerr(currentToken, "Missing statement after ELSE.");
	                        }

	                        return 1;
	                    }
	                    else
	                    	tkerr(currentToken, "Missing statement after ')'.");
	                }
	                else
	                	tkerr(currentToken, "Missing ')' after expression.");
	            }
	            else
	            	tkerr(currentToken, "Missing expression after '('.");
	        }
	        else 
	            tkerr(currentToken, "Missing '(' after IF.");
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

	                    else
	                    	tkerr(currentToken, "Missing statement.");
	                }
	                else
	                	tkerr(currentToken, "Missing ')' after expression.");
	            }
	            else 
	            	tkerr(currentToken, "Missing expression after '('.");
	        }
	        else 
	        	tkerr(currentToken, "Missing '(' after WHILE.");
	    }

	    else if (consume(FOR))
	    {
	        if (consume(LPAR))
	        {
	            expr();

	            if (consume(SEMICOLON))
	            {
	                expr();

	                if (consume(SEMICOLON))
	                {
	                    expr();

	                    if (consume(RPAR))
	                    {
	                        if (stm())
	                            return 1;

	                        else
	                        	tkerr(currentToken, "Missing statement.");
	                    }
	                    else
	                    	tkerr(currentToken, "Missing ')' after expression.");
	                }
	                else 
	                	tkerr(currentToken, "Missing ';' after expression.");
	            }
	            else
	            	tkerr(currentToken, "Missing ';' after expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing '(' after FOR clause.");
	    }

	    else if (consume(BREAK))
	    {
	        if (consume(SEMICOLON))
	            return 1;

	        else
	        	tkerr(currentToken, "Missing ';' after BREAK.");
	    }

	    else if (consume(RETURN))
	    {
	        expr();

	        if (consume(SEMICOLON))
	            return 1;

	        else
	        	tkerr(currentToken, "Missing ';' after RETURN.");
	    }

	    else if (expr())
	    {
	        if (consume(SEMICOLON))
	            return 1;

	        else
	        	tkerr(currentToken, "Missing ';' after expression.");
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
	        if (declStruct())
	        	;
	        else if (declFunc())
	        	;
	        else if (declVar())
	        	;
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

	            else
	            	tkerr(currentToken, "Incomplete expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing expression.");
	    }
	    
	    currentToken = startToken;
	    return 1;
	}

	int exprOr()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprOr %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprAnd())
	    {
	        if (exprOrPrim())
	            return 1;

	        else
	        	tkerr(currentToken, "Incomplete expression.");
	    }
	    
	    currentToken = startToken;
	    return 0;
	}

	int exprAnd()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprAnd %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprEq())
	    {
	        if (exprAndPrim())
	            return 1;

	        else
	        	tkerr(currentToken, "Incomplete expression.");
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

	            else
	            	tkerr(currentToken, "Incomplete expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing expression.");
	    }

	    currentToken = startToken;
	    return 1;
	}

	int exprEq()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprEq %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprRel())
	    {
	        if (exprEqPrim())
	            return 1;

	        else
	        	tkerr(currentToken, "Incomplete expression.");
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
	            if (exprEqPrim())
	                return 1;

	            else
	            	tkerr(currentToken, "Missing expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing expression.");
	    }

	    currentToken = startToken;
	    return 1;
	}

	int exprRelSecond()
	{
		if (consume(LESS))
			return 1;

		else if (consume(LESSEQ))
			return 1;

		else if (consume(GREATER))
			return 1;

		else if (consume(GREATEREQ))
			return 1;

		return 0;
	}

	int exprRelPrim()
	{
	    Token *startToken = currentToken;

	    if (exprRelSecond())
	    {
	        if (exprAdd())
	        {
	            if (exprRelPrim())
	                return 1;

	            else
	            	tkerr(currentToken, "Missing expression.");
	        }
	        else
	        	tkerr(currentToken, "Missing expression.");
	    }

	    currentToken = startToken;
	    return 1;
	}

	int exprRel()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprRel %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprAdd())
	    {
	        if (exprRelPrim())
	            return 1;

	        else
	        	tkerr(currentToken, "Incomplete expression.");
	    }

	    currentToken = startToken;
	    return 0;
	}

	int exprAdd()
	{
	    Token *startToken = currentToken;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprAdd %s?\n", getTokenName(currentToken->code));
	    #endif

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

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprMul %s?\n", getTokenName(currentToken->code));
	    #endif

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

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprPostfix %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (exprPrimary())
	    {
	        if (exprPostfixPrim())
	            return 1;

	        else
	        	tkerr(currentToken, "Incomplete postfix expression.");
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
	        else
	        	tkerr(currentToken, "Missing structure ID.");
	    }

	    currentToken = startToken;
	    return 1;
	}

	int exprCast()
	{
	    Token *startToken = currentToken;
	    Type t;

	    #if defined _DEBUG_MODE
	    	printf("[debug]: exprCast %s?\n", getTokenName(currentToken->code));
	    #endif

	    if (consume(LPAR))
	    {
	        if (typeName(&t))
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

	void initSymbols(Symbols *symbols)
	{
		symbols->begin = symbols->end = symbols->after = NULL;
	}
	
	Symbol *addSymbol(Symbols *symbols, const char *name, int cls)
	{
		Symbol *s;

		if (symbols->end == symbols->after)
		{
			int count = symbols->after - symbols->begin;
			int n = count * 2;
			
			if (!n)
				n = 1;
				
			if ((symbols->begin = (Symbol **) realloc(symbols->begin, n * sizeof(Symbol *))) == NULL)
				err("malloc() error.");
				
			symbols->end = symbols->begin + count;
			symbols->after = symbols->begin + n;
		}
		
		SAFEALLOC(s, Symbol, 1);
		*symbols->end++ = s;
		s->name = name;
		s->cls = cls;
		s->depth = currentDepth;

		return s;
	}
	
	Symbol *findSymbol(Symbols *symbols, const char *name)
	{
		int n = symbols->end - symbols->begin;
		int i;
		
		for (i = n - 1; i >= 0; i--)
		{
			if (!strcmp(symbols->begin[i]->name, name))
				return symbols->begin[i];
		}
		
		return NULL;
	}

	void deleteSymbolsAfter(Symbols *symbols, Symbol *start)
	{
		int pos, i;
		int n = symbols->end - symbols->begin;

		for (i = n - 1; i >= 0; i--)
		{
			if (!strcmp(symbols->begin[i]->name, start->name))
			{
				pos = i;
				break;
			}
		}

		printf("[debug]: deleteSymbolsAfter() = %d, n = %d\n", pos, n);

		for (i = pos + 1; i < n; i++)
		{
			//free(symbols->begin[i]);
			//symbols->begin[i] = NULL;
		}
	}
#endif