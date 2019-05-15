#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> // sleep

#include "common.c"
#include "utils.c"
#include "lex_analyzer.c"
#include "syn_analyzer.c"

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
		perror("file error");
		exit(EXIT_FAILURE);
	}

	int n = fread(buff, sizeof(char), BUFF_LEN, f);
	fclose(f);

	buff[n] = '\0';
	crtCh = buff;
	
	initSymbols(&symbols);
	
	while (getNextToken() != END);
	print_tokens();

	currentToken = h;

	if (unit())
		printf("syntax ok\n");

	else
		printf("syntax error\n");
		
	print_symbols(&symbols);

	return 0;
}