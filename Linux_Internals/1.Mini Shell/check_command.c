#include "main.h"

 // check the command is INTERNAL or EXTERNAL or NO_COMMAND 

int check_command_type(char *command)
{
    // List of builtin commands
    // Internal commands or Built_in commands are the commands which are built into your shell

    char *builtins[] = { "echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval", "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source", "exit", "exec",     "shopt", "caller", "true", "type", "hash","bind","help", NULL };

    int i;

    for(i=0; builtins[i] != NULL; i++)				// for loop to check whether the given command is in the list of built_in command or not
    {
	if(strcmp(command,builtins[i]) == 0)			// if condition to check whether the given command is Builtin command or not by using the strcmp function 
	{							// if the given command is built_in command then
	    return BUILTIN;					// then return the BUILTIN value 
	}
    }


    // To extract the  External commands
    // External commands are not Built_in commands 
    // External commands are the commands which are not in the shell , commands binary is avialble in external memory location
    // These commands are the Programms which have their own Binary file and located in Filesystem

    char *external_commands[155] = {NULL};				// declared an array of pointers and intialised to NULL and it is used to store the External commands

    extract_external_commands(external_commands);			// calling the extract_external_command function to extract the commands from the external_commands.txt file to store into the external commands array of pointers

    for(i=0; external_commands[i] != NULL; i++)				// for loop to check whehter the given command is in the list of external_commands or not
    {
	if(strcmp(command,external_commands[i]) == 0)			// if condition to check whether the given command is external_command or not by using the strcmp function
	{								// if the given command is External then
	    return EXTERNAL;						// return the value of the EXTERNAL macro
	}
    }


    // TO check whether you have Entered any command or not

    if(strcmp(command,"\0") == 0)		// Check the condition whether have you enterd any command or not
    {						// if you don't typed any command means then 
	return NO_COMMAND;			// return the value of the NO_COMMAND
    }

}

