#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> // sleep

#include "common.h"
#include "asin_functions.h"

#define BUFF_LEN 10000
#define SAFEALLOC(var,Type,count) if((var = (Type *) malloc(count * sizeof(Type))) == NULL) err("not enough memory")

char *crtCh;
char *buff;

Token *currentToken;

void print_debug(int state, char ch)
{
	printf("[DEBUG]: #%d, ", state);
	if (ch == '\n')
		printf("\\n, ");

	else if (ch == '\r')
		printf("\\r, ");

	else
		printf("%c, ", ch);

	printf("%d\n", ch);
}

char *createString(char *start, char *end)
{
	char *str;

	if ((str = (char *) malloc(end - start)) == NULL)
		err("malloc error.");

	int k = 0;
	while (start != end) 
	{
		str[k++] = *start;
		start ++;
	}

	return str;
}

Token *addTk(int code)
{
	Token *tk;
	SAFEALLOC(tk, Token, 1);
	tk->code = code;
	tk->line = line;
	tk->nxt = NULL;
	
	if (h == NULL)
		h = t = tk;

	else
	{
		t->nxt = tk;
		t = tk;
	}

	return tk;
}

void print_tokens()
{
	Token *p = h;

	for (; p != NULL; p = p->nxt)
	{
		printf("%d %s", p->line, getCode(p->code));

		if (p->code == CT_INT)
			printf(": %ld\n", p->i);
		
		else if (p->code == CT_REAL)
			printf(": %lf\n", p->r);

		else if (p->code == ID || p->code == CT_STRING)
			printf(": %s\n", p->text);

		else if (p->code == CT_CHAR)
			printf(": %c\n", p->c);

		else
			printf("\n");
	}
}

char escapeChar(char c)
{
	switch (c)
	{
		case 'n': return '\n';
		case 't': return '\t';
		case '\\': return '\\';
		case '"': return '\"';
		case 'r': return '\r';
		case 'b': return '\b';
		default: return c;
	}
	return 0;
}

char *escapeString(char *str)
{
	int len = strlen(str);
	int i, j;
	
	for (i = 0; i < len - 1; i++)
	{
		if (str[i] == '\\')
		{
			str[i] = escapeChar(str[i + 1]);
			
			for (j = i + 1; j < len; j++)
				str[j] = str[j + 1];
				
			len --;
		}
	}
	
	return str;
}

int getNextToken()
{
	int s = 0;
	char ch;
	char *startCh;
	Token *tk;

	for (;;)
	{
		ch = *crtCh;
		
		#if defined DEBUG_MODE
			print_debug(s, ch);
		#endif

		switch (s)
		{
			case 0:
			{
				if (isalpha(ch) || ch == '_')
				{
					startCh = crtCh;
					++crtCh;
					s = 1;
				}
				else if (ch >= '1' && ch <= '9')
				{
					startCh = crtCh;
					++crtCh;
					s = 3;
				}
				else if (ch == '0')
				{
					startCh = crtCh;
					++crtCh;
					s = 5;
				}
				else if (ch == '\'')
				{
					startCh = crtCh;
					++crtCh;
					s = 17;
				}
				else if (ch == '"')
				{
					startCh = crtCh;
					++crtCh;
					s = 21;
				}
				else if (ch == '=')
				{
					++crtCh;
					s = 51;
				}
				else if (ch == ' ' || ch == '\r' || ch == '\t')
					++crtCh;

				else if (ch == '\n')
				{
					++line;
					++crtCh;
				}
				else if (ch == ',')
				{
					s = 25;
					++crtCh;
				}
				
				else if (ch == ';')
				{
					s = 26;
					++crtCh;
				}
				else if (ch == '(')
				{
					s = 27;
					++crtCh;
				}
				else if (ch == ')')
				{
					s = 28;
					++crtCh;
				}
				else if (ch == '[')
				{
					s = 29;
					++crtCh;
				}
				else if (ch == ']')
				{
					s = 30;
					++crtCh;
				}
				else if (ch == '{')
				{
					s = 31;
					++crtCh;
				}
				else if (ch == '}')
				{
					s = 32;
					++crtCh;
				}
				else if (ch == '-')
				{
					s = 33;
					++crtCh;
				}
				else if (ch == '+')
				{
					s = 34;
					++crtCh;
				}
				else if (ch == '*')
				{
					s = 35;
					++crtCh;
				}
				else if (ch == '/')
				{
					s = 36;
					++crtCh;
				}
				else if (ch == '.')
				{
					s = 43;
					++crtCh;
				}
				else if (ch == '&')
				{
					s = 44;
					++crtCh;
				}
				else if (ch == '|')
				{
					s = 46;
					++crtCh;
				}
				else if (ch == '!')
				{
					s = 48;
					++crtCh;
				}
				else if (ch == '<')
				{
					s = 54;
					++crtCh;
				}
				else if (ch == '>')
				{
					s = 57;
					++crtCh;
				}
				else if (ch == '\0')
				{
					addTk(END);
					return END;
				}
				
				else
					err("invalid character %c\n", crtCh);
					
				break;
			}
			case 1:
			{
				if (isalnum(ch) || ch == '_')
					++crtCh;
		
				else
					s = 2;
					
				break;
			}
			case 2:
			{
				if (!memcmp(startCh, "if", 2)) tk = addTk(IF);
				else if (!memcmp(startCh, "for", 3)) tk = addTk(FOR);
				else if (!memcmp(startCh, "int", 3)) tk = addTk(INT);
				else if (!memcmp(startCh, "char", 4)) tk = addTk(CHAR);
				else if (!memcmp(startCh, "void", 4)) tk = addTk(VOID);
				else if (!memcmp(startCh, "else", 4)) tk = addTk(ELSE);
				else if (!memcmp(startCh, "break", 5)) tk = addTk(BREAK);
				else if (!memcmp(startCh, "while", 5)) tk = addTk(WHILE);
				else if (!memcmp(startCh, "double", 6)) tk = addTk(DOUBLE);
				else if (!memcmp(startCh, "return", 6)) tk = addTk(RETURN);
				else if (!memcmp(startCh, "struct", 6)) tk = addTk(STRUCT);
				else
				{
					tk = addTk(ID);
					
					SAFEALLOC(tk->text, char, (int)(crtCh - startCh));
					tk->text = createString(startCh, crtCh);
				}
				
				return tk->code;
			}
			case 3:
			{
				if (isdigit(ch))
					++crtCh;

				else if (ch == '.')
				{
					++crtCh;
					s = 10;
				}

				else if (ch == 'E' || ch == 'e')
				{
					++crtCh;
					s = 13;
				}
					
				else
					s = 4;
					
				break;
			}
			case 4:
			{
				tk = addTk(CT_INT);

				if (startCh[0] == '0' && startCh[1] == 'x')
					tk->i = strtol(createString(startCh, crtCh), NULL, 0);
					
				else if (startCh[0] == '0')
					tk->i = strtol(createString(startCh, crtCh), NULL, 8);
					
				else
					tk->i = strtol(createString(startCh, crtCh), NULL, 10);
				
				return tk->code;
			}
			case 5:
			{
				if (ch == 'x')
				{
					++crtCh;
					s = 7;
				}
				else if (ch == '.')
				{
					++crtCh;
					s = 10;
				}
				else
					s = 6;

				break;
			}
			case 6:
			{
				if (ch >= '0' && ch <= '9')
				{
					++crtCh;

					if (ch == '8' || ch == '9')
						s = 9;
				}

				else if (ch == '.')
				{
					++crtCh;
					s = 10;
				}

				else
					s = 4;

				break;
			}
			case 7:
			{
				if (isdigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
				{
					++crtCh;
					s = 8;
				}

				break;
			}
			case 8:
			{
				if (isdigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
					++crtCh;

				else
					s = 4;

				break;
			}
			case 9:
			{
				if (isdigit(ch))
					++crtCh;

				else if (ch == '.')
				{
					++crtCh;
					s = 10;
				}

				else if (ch == 'E' || ch == 'e')
				{
					++crtCh;
					s = 13;
				}

				break;
			}
			case 10:
			{
				if (isdigit(ch))
				{
					++crtCh;
					s = 11;
				}
				break;
			}
			case 11:
			{
				if (isdigit(ch))
					++crtCh;
				
				else
					s = 12;

				break;
			}
			case 12:
			{
				if (ch == 'E' || ch == 'e')
				{
					++crtCh;
					s = 13;
				}
				else
					s = 16;

				break;
			}
			case 13:
			{
				if (ch == '+' || ch == '-')
				{
					++crtCh;
					s = 14;
				}
				else
					s = 14;

				break;
			}
			case 14:
			{
				if (isdigit(ch))
				{
					++crtCh;
					s = 15;
				}

				break;
			}
			case 15:
			{
				if (isdigit(ch))
				{
					++crtCh;
					s = 16;
				}
				else
					s = 16;

				break;
			}
			case 16:
			{
				tk = addTk(CT_REAL);
				tk->r = atof(createString(startCh, crtCh));
				return CT_REAL;
			}
			case 17:
			{
				if (ch == '\\')
				{
					++crtCh;
					s = 18;
				}
				else if (!(ch == '^' || ch == '\'' || ch == '\\'))
				{
					startCh = crtCh;
					++crtCh;
					s = 19;
				}

				break;
			}
			case 18:
			{
				if (strchr("abfnrtv'?\"\\\0", ch) != NULL)
				{
					startCh = crtCh;
					++crtCh;
					s = 19;
				}

				break;
			}
			case 19:
			{
				if (ch == '\'')
				{
					++crtCh;
					s = 20;
				}

				break;
			}
			case 20:
			{
				tk = addTk(CT_CHAR);
				tk->c = *startCh;

				return CT_CHAR;
			}
			case 21:
			{
				if (!(ch == '"' || ch == '\\'))
					++crtCh;

				else if (ch == '\\')
				{
					++crtCh;
					s = 22;
				}
				
				else if (ch == '"')
				{
					++crtCh;
					s = 24;
				}

				break;
			}
			case 22:
			{
				if (strchr("abfnrtv'?\"\\\0", ch) != NULL)
				{
					++crtCh;
					s = 21;
				}

				break;
			}
			case 24:
			{
				tk = addTk(CT_STRING);
				tk->text = escapeString(createString(startCh + 1, crtCh - 1));
				
				return CT_STRING;
			}
			case 25:
			{
				addTk(COMMA);
				return COMMA;
			}
			case 26:
			{
				addTk(SEMICOLON);
				return SEMICOLON;
			}
			case 27:
			{
				addTk(LPAR);
				return LPAR;
			}
			case 28:
			{
				addTk(RPAR);
				return RPAR;
			}
			case 29:
			{
				addTk(LBRA);
				return LBRA;
			}
			case 30:
			{
				addTk(RBRA);
				return RBRA;
			}
			case 31:
			{
				addTk(LACC);
				return LACC;
			}
			case 32:
			{
				addTk(RACC);
				return RACC;
			}
			case 33:
			{
				addTk(SUB);
				return SUB;
			}
			case 34:
			{
				addTk(ADD);
				return ADD;
			}
			case 35:
			{
				addTk(MUL);
				return MUL;
			}
			case 36:
			{
				if (ch == '/')
				{
					++crtCh;
					s = 38;
				}
				else if (ch == '*')
				{
					++crtCh;
					s = 40;
				}
				else
					s = 37;

				break;
			}
			case 37:
			{
				addTk(DIV);
				return DIV;
			}
			case 38:
			{
				if (!(ch == '\n' || ch == '\r' || ch == '\0'))
					++crtCh;

				else
					s = 39;

				break;
			}
			case 39:
			{
				s = 0;
				break;
			}
			case 40:
			{
				if (ch != '*')
				{
					if (ch == '\n')
						++line;
						
					++crtCh;
				}
				
				else if (ch == '*')
				{
					++crtCh;
					s = 41;
				}

				break;
			}
			case 41:
			{
				if (ch == '*')
					++ crtCh;
				
				else if (ch == '/')
				{
					++ crtCh;
					s = 42;
				}
				else
				{
					++ crtCh;
					s = 40;
				}

				break;
			}
			case 42:
			{
				s = 0;
				break;
			}
			case 43:
			{
				addTk(DOT);
				return DOT;
			}
			case 44:
			{
				if (ch == '&')
				{
					++ crtCh;
					s = 45;
				}
				break;
			}
			case 45:
			{
				addTk(AND);
				return AND;
			}
			case 46:
			{
				if (ch == '|')
				{
					++crtCh;
					s = 47;
				}
				break;
			}
			case 47:
			{
				addTk(OR);
				return OR;
			}
			case 48:
			{
				if (ch == '=')
				{
					++ crtCh;
					s = 50;
				}
				else
					s = 49;

				break;
			}
			case 49:
			{
				addTk(NOT);
				return NOT;
			}
			case 50:
			{
				addTk(NOTEQ);
				return NOTEQ;
			}
			case 51:
			{
				if (ch == '=')
				{
					++crtCh;
					s = 53;
				}
				else
					s = 52;

				break;
			}
			case 52:
			{
				addTk(ASSIGN);
				return ASSIGN;
			}
			case 53:
			{
				addTk(EQUAL);
				return EQUAL;
			}
			case 54:
			{
				if (ch == '=')
				{
					++ crtCh;
					s = 56;
				}
				else
					s = 55;

				break;
			}
			case 55:
			{
				addTk(LESS);
				return LESS;
			}
			case 56:
			{
				addTk(LESSEQ);
				return LESSEQ;
			}
			case 57:
			{
				if (ch == '=')
				{
					++ crtCh;
					s = 59;
				}
				else
					s = 58;

				break;
			}
			case 58:
			{
				addTk(GREATER);
				return GREATER;
			}
			case 59:
			{
				addTk(GREATEREQ);
				return GREATEREQ;
			}
		}

		// sleep(1.3);
	}
}

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

    printf("[debug]: expr %s?\n", getCode(currentToken->code));
    
    if (exprAssign())
        return 1;
        
    currentToken = startToken;
    return 0;
}

int exprPrimary()
{
    Token *startToken = currentToken;

    printf("[debug]: exprPrimary %s?\n", getCode(currentToken->code));
    
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

    printf("[debug]: exprUnary %s?\n", getCode(currentToken->code));

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

	printf("[debug]: exprAssign %s?\n", getCode(currentToken->code));

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

int arrayDecl()
{
    Token *startToken = currentToken;

    printf("[debug]: arrayDecl %s?\n", getCode(currentToken->code));
    
    if (consume(LBRA))
    {
        expr();
        
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
    
    printf("[debug]: declStruct %s?\n", getCode(currentToken->code));
    
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            if (consume(LACC))
            {
                for (;;)
                {
                    if (declVar())
                    	continue;

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

    printf("[debug]: typeBase %s?\n", getCode(currentToken->code));

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

        else
        	tkerr(currentToken, "Missing ID after STRUCT.");
    }
    
    currentToken = startToken;
    return 0;
}

int typeName()
{
    if (typeBase())
    {
        arrayDecl();
        return 1;
    }
    return 0;
}

int funcArg()
{
    Token *startToken = currentToken;

    printf("[debug]: funcArg %s?\n", getCode(currentToken->code));
    
    if (typeBase())
    {
        if (consume(ID))
        {
            arrayDecl();
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
    
    printf("[debug]: declFunc %s?\n", getCode(currentToken->code));
    
    if (typeBase())
    {
    	consume(MUL);
       	goto DECLFUNC;
    }

    else if (consume(VOID))
    	goto DECLFUNC;

    DECLFUNC:
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
                if (stmCompound())
                    return 1;

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

    printf("[debug]: declVar %s?\n", getCode(currentToken->code));

    if (typeBase())
    {
        if (consume(ID))
        {
            arrayDecl();
            for (;;)
            {
                if (consume(COMMA))
                {
                    if (consume(ID))
						arrayDecl();
                    
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

    printf("[debug]: stmCompound %s?\n", getCode(currentToken->code));

    if (consume(LACC))
    {
        for (;;)
        {
            if (declVar())
            	continue;

            else if (stm())
            	continue;

            else break;
        }

        if (consume(RACC))
            return 1;

        else
        	tkerr(currentToken, "Missing }.");
    }

    currentToken = startToken;
    return 0;
}

int stm()
{
    Token *startToken = currentToken;

    printf("[debug]: stm %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprOr %s?\n", getCode(currentToken->code));
    
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

    printf("[debug]: exprAnd %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprEq %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprRel %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprAdd %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprMul %s?\n", getCode(currentToken->code));

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

    printf("[debug]: exprPostfix %s?\n", getCode(currentToken->code));

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
    }

    currentToken = startToken;
    return 1;
}

int exprCast()
{
    Token *startToken = currentToken;

    printf("[debug]: exprCast %s?\n", getCode(currentToken->code));

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

int main(int argc, char *argv[])
{
	if ((buff = (char *) malloc(BUFF_LEN + 1)) == NULL)
	{
		perror("malloc error.");
		exit(EXIT_FAILURE);
	}

	FILE *f;
	if ((f = fopen("9.c", "r")) == NULL)
	{
		perror("file error.");
		exit(EXIT_FAILURE);
	}

	int n = fread(buff, sizeof(char), BUFF_LEN, f);
	fclose(f);

	buff[n] = '\0';
	crtCh = buff;
	
	while (getNextToken() != END);
	print_tokens();

	currentToken = h;

	if (unit())
		printf("syntax ok\n");

	else
		printf("syntax error\n");

	return 0;
}