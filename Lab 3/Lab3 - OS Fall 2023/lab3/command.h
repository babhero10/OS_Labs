
#ifndef command_h
#define command_h

// Command Data Structure
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;
	
	// Number of arguments
	int _numberOfArguments;
	char ** _arguments;
	
	SimpleCommand();
	void insertArgument( char * argument );
};

struct Command {
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	SimpleCommand ** _simpleCommands;
	unsigned char _append;
	char * _outFile;
	char * _inputFile;
	char * _errFile;
	char _path[1024];
	int _background;

	void prompt();
	void print();
	void execute_helper();
	void execute();
	void clear();
	char special_command(char *command_name, char **args);
	void update_path();
	Command();
	void insertSimpleCommand( SimpleCommand * simpleCommand );

	static Command _currentCommand;
	static SimpleCommand *_currentSimpleCommand;
};

#endif
