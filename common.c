#ifndef _COMMON_CODE_INCLUDE
	#define _COMMON_CODE_INCLUDE
	
	#ifndef _DEBUG_MODE
		// #define _DEBUG_MODE
	#endif
	
	#define BUFF_LEN 10000

	const char *codes[40] = 
	{
		"ID", "END", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRA", "RBRA", 
		"LACC", "RACC", "ADD", "SUB", "DIV", "MUL", "DOT", "ASSIGN", "EQUAL", "NOTEQ", "AND", "OR", "NOT", "LESS", "LESSEQ", 
		"GREATER", "GREATEREQ", "BREAK", "CHAR", "VOID", "INT", "FOR", "WHILE", "RETURN", "STRUCT", "DOUBLE", "IF", "ELSE"
	};

	const char *classes[4] =
	{
		"CLS_VAR", "CLS_FUNC", "CLS_EXTFUNC", "CLS_STRUCT"
	};

	enum
	{
		ID = 0, 
		END, 
		CT_INT, CT_REAL, CT_CHAR, CT_STRING,
		COMMA, SEMICOLON, LPAR, RPAR, LBRA, RBRA, LACC, RACC, ADD, SUB, DIV, MUL, DOT,
		ASSIGN, EQUAL, NOTEQ, AND, OR, NOT, LESS, LESSEQ, GREATER, GREATEREQ,
		BREAK, CHAR, VOID, INT, FOR, WHILE, RETURN, STRUCT, DOUBLE, IF, ELSE
	};
	enum
	{
		TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID
	};
	enum
	{
		CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT
	};
	enum
	{
		MEM_GLOBAL, MEM_ARG, MEM_LOCAL
	};
	
	typedef struct _Symbol Symbol;
	
	typedef struct
	{
		int typeBase;
		Symbol *s;
		int nElements;
	} Type;
	
	typedef struct
	{
		Symbol **begin;
		Symbol **end;
		Symbol **after;
	} Symbols;
	
	typedef struct _Symbol
	{
		const char *name;
		int cls;
		int mem;
		Type type;
		int depth;
		union
		{
			Symbols args;
			Symbols members;
		};
	} Symbol;

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

	char *crtCh;
	char *buff;

	int line = 1;
	int currentDepth = 0;
	
	Token *h = NULL;
	Token *t = NULL;
	Token *consumedToken;
	Token *currentToken;
	Symbols symbols;
	Symbol *currentFunc;
	Symbol *currentStruct;
#endif
