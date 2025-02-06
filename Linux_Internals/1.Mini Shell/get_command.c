#include "main.h"

// Extract the command untill space or New-line Encountered

char *get_command(char *input_string)
{
    // Declare an array to store command

    static char command[25] = {'\0'};			// Initilalize the all bytes to NULL character

    int i = 0;

    while(1)
    {
	if(*input_string == ' ' || *input_string == '\0')	// check condition whether it reaches to ' ' or '\0' in the input string 
	{							// if the above condition is true means then 
	    break;						// break the loop
	}
	command[i++] = *input_string;				// storing that command from the input_string to command array
	input_string++;						// incrementing the input_string to it's next charachter 

    }

    command[i] = '\0';						// Appending the NULL character at the end of the command

    return command;
}
