#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> // sleep

#include "functions.c"

#define BUFF_LEN 10000
#define DEBUG_MODE

char *crtCh;
char *buff;

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
					err("caracter invalid %c\n", crtCh);
					
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
				int chars = crtCh - startCh;
				printf("no of chars %d\n", chars);

				if (chars == 5)
				{
					if (!memcmp(startCh, "break", 5))
						tk = addTk(BREAK);

					else if (!memcmp(startCh, "while", 5))
						tk = addTk(WHILE);

					else
					{
						tk = addTk(ID);
						
						SAFEALLOC(tk->text, char, chars);
						tk->text = createString(startCh, crtCh);
					}
				}
				else if (chars == 2)
				{
					if (!memcmp(startCh, "if", 2))
						tk = addTk(IF);

					else
					{
						tk = addTk(ID);
						
						SAFEALLOC(tk->text, char, chars);
						tk->text = createString(startCh, crtCh);
					}
				}
				else if (chars == 3)
				{
					if (!memcmp(startCh, "for", 3))
						tk = addTk(FOR);

					else if (!memcmp(startCh, "int", 3))
						tk = addTk(INT);

					else
					{
						tk = addTk(ID);
						
						SAFEALLOC(tk->text, char, chars);
						tk->text = createString(startCh, crtCh);
					}
				}
				else if (chars == 4)
				{
					if (!memcmp(startCh, "char", 4))
						tk = addTk(CHAR);
 
					else if (!memcmp(startCh, "else", 4))
						tk = addTk(ELSE);

					else if (!memcmp(startCh, "void", 4))
						tk = addTk(VOID);

					else
					{
						tk = addTk(ID);

						SAFEALLOC(tk->text, char, chars);
						tk->text = createString(startCh, crtCh);
					}
				}
				else if (chars == 6)
				{
					if (!memcmp(startCh, "double", 6))
						tk = addTk(DOUBLE);

					else if (!memcmp(startCh, "return", 6))
						tk = addTk(RETURN);

					else if (!memcmp(startCh, "struct", 6))
						tk = addTk(STRUCT);

					else
					{
						tk = addTk(ID);
						
						SAFEALLOC(tk->text, char, chars);
						tk->text = createString(startCh, crtCh);
					}
				}
				else
				{
					tk = addTk(ID);
					
					SAFEALLOC(tk->text, char, chars);
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

				if (!memcmp(startCh, "0x", 2))
					tk->i = strtol(createString(startCh, crtCh), NULL, 0);

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
				if (ch >= '0' && ch <= '7')
					++crtCh;

				else if (ch == '8' || ch == '9')
				{
					++crtCh;
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
				if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '\0')
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
				if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '\0')
				{
					++crtCh;
					s = 21;
				}

				break;
			}
			case 24:
			{
				tk = addTk(CT_STRING);
				tk->text = createString(startCh, crtCh);
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
					++crtCh;
				
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

int main(int argc, char *argv[])
{
	if ((buff = (char *) malloc(BUFF_LEN + 1)) == NULL)
	{
		perror("malloc error.");
		exit(EXIT_FAILURE);
	}

	FILE *f;
	if ((f = fopen("testAnalizor.c", "r")) == NULL)
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

	return 0;
}