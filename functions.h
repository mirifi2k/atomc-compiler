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

void err(const char *fmt, ...);
void tkerr(const Token *tk, const char *fmt, ...);
void print_debug(int state, char ch);
Token *addTk(int code);
char *createString(char *start, char *end);
char *getCode(int code);
void print_tokens();