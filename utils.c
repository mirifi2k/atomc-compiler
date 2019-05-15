#ifndef _UTILS_INCLUDE
	#define _UTILS_INCLUDE

	void err(const char *fmt, ...);
	void tkerr(const Token *tk, const char *fmt, ...);

	#define SAFEALLOC(var,Type,count) if((var = (Type *) malloc(count * sizeof(Type))) == NULL) err("not enough memory")

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
		fputc('\n', stderr);
		va_end(va);
		exit(-1);
	}

	const char *getTokenName(int code)
	{
		return codes[code];
	}
	
	const char *getClassName(int class)
	{
		return classes[class];
	}

	const char *getTypeBase(int type)
	{
		return typeBases[type];
	}

	const char *getMemoryType(int type)
	{
		return memoryTypes[type];
	}

	#ifdef _DEBUG_MODE
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
	#endif

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

	void print_tokens()
	{
		Token *p = h;

		for (; p != NULL; p = p->nxt)
		{
			printf("%d %s", p->line, getTokenName(p->code));

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

	void print_symbols(Symbols *symbols)
	{
		int i;
		int n = symbols->end - symbols->begin;
		
		for (i = 0; i < n; i++)
		{
			printf("%s | %s | %s | %s\n", getTypeBase(symbols->begin[i]->type.typeBase), getClassName(symbols->begin[i]->cls), getMemoryType(symbols->begin[i]->mem), symbols->begin[i]->name);
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
#endif