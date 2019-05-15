#ifndef _COMMON_CODE_INCLUDE
	#define _COMMON_CODE_INCLUDE
	
	#ifndef _DEBUG_MODE
		#define _DEBUG_MODE
	#endif
	
	#define BUFF_LEN 10000
	#define SAFEALLOC(var,Type,count) if((var = (Type *) malloc(count * sizeof(Type))) == NULL) err("not enough memory")

	const char *codes[40] = 
	{
		"ID", "END", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRA", "RBRA", 
		"LACC", "RACC", "ADD", "SUB", "DIV", "MUL", "DOT", "ASSIGN", "EQUAL", "NOTEQ", "AND", "OR", "NOT", "LESS", "LESSEQ", 
		"GREATER", "GREATEREQ", "BREAK", "CHAR", "VOID", "INT", "FOR", "WHILE", "RETURN", "STRUCT", "DOUBLE", "IF", "ELSE"
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
		fputc('\n', stderr);
		va_end(va);
		exit(-1);
	}

	const char *getCode(int code)
	{
		return codes[code];
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
	
	void printSymbols(Symbols *symbols)
	{
		int i;
		int n = symbols->end - symbols->begin;
		
		for (i = 0; i < n; i++)
		{
			printf("%s\n", symbols->begin[i]->name);
		}
	}
	
	Symbol *findSymbol(Symbols *symbols, const char *name)
	{
		int n = symbols->end - symbols->begin;
		int i;
		
		for (i = n - 1; i >= 0; i--)
		{
			if (symbols->begin[i]->name == name)
				return symbols->begin[i];
		}
		
		return NULL;
	}
#endif
