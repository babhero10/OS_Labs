
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"
#include <signal.h>

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	


">" {	
		return OUT_REDIRECT;
	}

">>" {	
		return OUT_APPEND;
	}

"<" {	
		return IN_REDIRECT;
	}

"|" {	
		return PIPE;
	}

"&" {
		return BACKGROUND;
	}

[^ \t\n><|&]+|"\""[^\"]*"\"" {
    if (yytext[0] == '\"') {
        yylval.string_val = strdup(yytext + 1);
        yylval.string_val[strlen(yylval.string_val) - 1] = '\0';
    } else {
        yylval.string_val = strdup(yytext);
    }
    return WORD;
}

"*"    { yylval.string_val = strdup(yytext); return WILDCARD; }
"?"    { yylval.string_val = strdup(yytext); return WILDCARD; }

.	{
		/* Invalid Syntax */
		return NOTOKEN;
	}

%%