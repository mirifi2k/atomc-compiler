#include "functions.h"

#define SAFEALLOC(var,Type,count) if((var = (Type *) malloc(count * sizeof(Type))) == NULL) err("not enough memory")

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

void err(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error in line %d: ", tk->line);
	vfprintf(stderr, fmt, va);
	putc('\n', stderr);
	va_end(va);
	exit(-1);
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

char *getCode(int code)
{
	switch (code)
	{
		case ID: return "ID";
		case END: return "END";
		case CT_INT: return "CT_INT";
		case CT_REAL: return "CT_REAL";
		case CT_CHAR: return "CT_CHAR";
		case CT_STRING: return "CT_STRING";
		case COMMA: return "COMMA";
		case SEMICOLON: return "SEMICOLON";
		case LPAR: return "LPAR";
		case RPAR: return "RPAR";
		case LBRA: return "LBRA";
		case RBRA: return "RBRA";
		case LACC: return "LACC";
		case RACC: return "RACC";
		case ADD: return "ADD";
		case SUB: return "SUB";
		case MUL: return "MUL";
		case DIV: return "DIV";
		case DOT: return "DOT";
		case ASSIGN: return "ASSIGN";
		case EQUAL: return "EQUAL";
		case NOTEQ: return "NOTEQ";
		case AND: return "AND";
		case OR: return "OR";
		case NOT: return "NOT";
		case LESS: return "LESS";
		case LESSEQ: return "LESSEQ";
		case GREATER: return "GREATER";
		case GREATEREQ: return "GREATEREQ";
		case BREAK: return "BREAK";
		case CHAR: return "CHAR";
		case VOID: return "VOID";
		case INT: return "INT";
		case FOR: return "FOR";
		case WHILE: return "WHILE";
		case DOUBLE: return "DOUBLE";
		case STRUCT: return "STRUCT";
		case IF: return "IF";
		case ELSE: return "ELSE";
	}
	return NULL;
}

void print_tokens()
{
	Token *p = h;
	int i = 1;

	for (; p != NULL; p = p->nxt)
	{
		printf("(LINE #%d) %d. %s", p->line, i, getCode(p->code));

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
		
		i ++;
	}
}