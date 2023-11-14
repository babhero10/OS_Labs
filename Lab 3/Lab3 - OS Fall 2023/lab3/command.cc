
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <glob.h>
#include <ctime>

#include "command.h"

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

int create_file(char *file_name, char mode)
{
	int file;
	if (access(file_name, F_OK) == -1)
	{
		file = creat(file_name, 0666);
		if (file < 0 ) {
			perror("creat");
			return -1;
		}
	}
	else
	{
		// File out write (0) or File in (2)
		if (mode == 0 || mode == 2)
		{
			file = open(file_name, O_RDWR);
		}
		else // File out append (1) 
		{
			file = open(file_name, O_RDWR | O_APPEND);
		}
		
		if (file < 0 ) {
			perror("open");
			return -1;
		}
	}

	return file;
}

char Command::special_command(char *command_name, char **args)
{  

	if (strcmp(command_name, "cd") == 0)
	{
		char dir[1024];

		if (_currentSimpleCommand->_numberOfArguments == 1 || args[1][0] == ' ')
		{
			strcpy(dir, "/");	
		}
		else
		{
			strcpy(dir, args[1]);	
		}

		if (chdir(dir) != 0) 
		{
			perror("myshell: exec chdir");
			return 2;
		}

		kill(getpid(), SIGCHLD);
		return 1;
	}
	
	return 0;
}

void handleSIGCHLD(int sig_num)
{
	FILE *log_file = fopen("log.txt", "a");

    if (log_file == NULL) {
        perror("myshell: open log");
        return;
    }

	for (int i = 0; i < Command::_currentSimpleCommand->_numberOfArguments ; i++)
	{	
		fprintf(log_file, "%s ", Command::_currentSimpleCommand->_arguments[i]);
	}

	fputc('\n', log_file);
	fclose(log_file);
}

void Command::execute_helper()
{
    int defaultIn = dup(0);
    int defaultOut = dup(1);
    int defaultErr = dup(2);
    int fdpipe[2];
	
	int prevRead = dup(defaultIn);
	int nextWrite;
	int fderr = dup(defaultErr);

	char *command_name;
    char **command_args;

    for (int current_command_index = 0; current_command_index < _currentCommand._numberOfSimpleCommands; current_command_index++)
    {

		command_name = _currentCommand._simpleCommands[current_command_index]->_arguments[0];
        command_args = _currentCommand._simpleCommands[current_command_index]->_arguments;

		if (special_command(command_name, command_args) == 1) {
			continue;
		}

		if (pipe(fdpipe) == -1)
		{
			perror("myshell: pipe");
			return;
		}

		if (current_command_index >= _currentCommand._numberOfSimpleCommands - 1) {
            nextWrite = dup(defaultOut);
        } else {
            nextWrite = dup(fdpipe[1]);
			close(fdpipe[1]);
        }
		
		if (current_command_index >= _currentCommand._numberOfSimpleCommands - 1) {
			if (_currentCommand._inputFile)
			{
				prevRead = create_file(_currentCommand._inputFile, 2);
				if (prevRead == -1)
				{
					fprintf(stderr, "myshell: create inFile\n");
					return;
				}
			}

			if (_currentCommand._outFile)
			{
				nextWrite = create_file(_currentCommand._outFile, _currentCommand._append);
				if (nextWrite == -1)
				{
					fprintf(stderr, "myshell: create outFile\n");
					return;
				}
			}

			if (_currentCommand._errFile)
			{
				fderr = create_file(_currentCommand._errFile, _currentCommand._append);
				if (fderr == -1)
				{
					fprintf(stderr, "myshell: create errFile\n");
					return;
				}
			}
		}

        dup2(prevRead, 0);
        dup2(nextWrite, 1);
        dup2(fderr, 2);

        close(prevRead);
        close(nextWrite);
		close(fderr);

        command_args = _currentCommand._simpleCommands[current_command_index]->_arguments;

        int pid = fork();
        if (pid == -1)
        {
            fprintf(stderr, "myshell: fork %s\n", command_name);
            return;
        }

        if (pid == 0)
		{
			// kill(getpid(), SIGINT);
			execvp(command_name, command_args);
			fprintf(stderr, "myshell: exec %s\n", command_name);
			return;
        }
		else
		{
			signal(SIGCHLD, handleSIGCHLD);
			if (!_currentCommand._background)
			{
				waitpid(pid, 0, 0);
			}

			prevRead = dup(fdpipe[0]);
			close(fdpipe[0]);
		}

    }

    dup2(defaultIn, 0);
    dup2(defaultOut, 1);
    dup2(defaultErr, 2);

	close(fdpipe[0]);
	close(fdpipe[1]);
	close(prevRead);
	close(nextWrite);
	close(fderr);
    close(defaultIn);
    close(defaultOut);
    close(defaultErr);
}

void Command::update_path()
{
	if (getcwd(_path, sizeof(_path)) == NULL) {
        perror("myshell: exec getcwd");
        return;
    }
}

void
Command::execute()
{

	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Execute
	execute_helper();

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	Command::_currentCommand.update_path();
	printf("myshell:~%s$ ", _path);
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

void sigintHandler(int signum) {
	printf("\n\n\tGood Bye!!\n\n");
	exit(0);	
}

int 
main()
{
	if (signal(SIGINT, sigintHandler) == SIG_ERR) {
		fprintf(stderr, "myshell: Failed to set signal handler for SIGINT\n");
		return 1;
	}

	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

