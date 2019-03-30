#ifndef _COMMON_CODE_INCLUDE
	#define _COMMON_CODE_INCLUDE
	#define DEBUG_MODE

	enum
	{
		ID = 0, 
		END, 
		CT_INT, CT_REAL, CT_CHAR, CT_STRING,
		COMMA, SEMICOLON, LPAR, RPAR, LBRA, RBRA, LACC, RACC, ADD, SUB, DIV, MUL, DOT,
		ASSIGN, EQUAL, NOTEQ, AND, OR, NOT, LESS, LESSEQ, GREATER, GREATEREQ,
		BREAK, CHAR, VOID, INT, FOR, WHILE, RETURN, STRUCT, DOUBLE, IF, ELSE
	};

	typedef struct Token
	{
		int code;
		union 
		{
			char *text;
			long int i;
			char c;
			double r;
		};
		int line;
		struct Token *nxt;
	} Token;

	int line = 1;
	Token *h = NULL;
	Token *t = NULL;

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

#endif