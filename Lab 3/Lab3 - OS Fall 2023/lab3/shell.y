/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */


%union	{
		char   *string_val;
	}

%token	<string_val> WORD
%token	<string_val> WILDCARD

%token 	NOTOKEN PIPE OUT_APPEND OUT_REDIRECT IN_REDIRECT NEWLINE 
%token 	ERR_APPEND ERR_REDIRECT BACKGROUND EXIT

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}

#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <glob.h>
#include <string.h>
%}

%{
	extern int yylineno; /* Initialize yylineno to 1 */
%}

%%

goal:	
	commands
	;

commands: 
    command
	| commands command
    ;

command:
    simple_command
    | command PIPE simple_command
    ;

simple_command:	
	command_and_args iomodifier_opt background_task NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	} | command_and_args iomodifier_opt background_task {
		printf("   Yacc: Add sample command\n");
	} | EXIT NEWLINE {
		printf("\n\n\tGood Bye!!\n\n");
		exit(0);
	}
	| error NEWLINE { yyerrok; }
	| NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
    WILDCARD {
        printf("   Yacc: insert wildcard argument \"%s\"\n", $1);

		glob_t globResult;
		int flags = GLOB_NOCHECK | GLOB_TILDE;
		if (glob($1, flags, NULL, &globResult) == 0) {
			
			for (size_t j = 1; j < globResult.gl_pathc; j++) {
				Command::_currentSimpleCommand->insertArgument(strdup(globResult.gl_pathv[j]));
			}

			globfree(&globResult);
		}

    } | WORD {
        printf("   Yacc: insert argument \"%s\"\n", $1);
        Command::_currentSimpleCommand->insertArgument($1);
    }
    ;


command_word:
	WORD {
            printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

background_task:
	BACKGROUND {
		Command::_currentCommand._background = 1;
	}
	| /* can be empty */
	;

iomodifier_opt:
    iomodifier_opt OUT_REDIRECT WORD {
        printf("   Yacc: write output \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._append = 0;
    }
    | iomodifier_opt OUT_APPEND WORD {
        printf("   Yacc: append output \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._append = 1;
    }
    | iomodifier_opt ERR_REDIRECT WORD {
        printf("   Yacc: write output and error \"%s\"\n", $3);
        Command::_currentCommand._errFile = $3;
    }
    | iomodifier_opt ERR_APPEND WORD {
        printf("   Yacc: append output and error \"%s\"\n", $3);
        Command::_currentCommand._errFile = $3;
    }
    | iomodifier_opt IN_REDIRECT WORD {
        printf("   Yacc: insert input \"%s\"\n", $3);
        Command::_currentCommand._inputFile = $3;
    }
    | /* can be empty */
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}

#if 0
main()
{
	yyparse();
}
#endif