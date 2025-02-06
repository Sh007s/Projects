#include "main.h"

// execute_internal_commands Function Defintion

void execute_internal_commands(char *input_string)
{
    // In this FUnction we are Implementing the Following Internal Commands
    // 1. exit  --> whenever you call this exit system call It terminate the execution of the current Process/program
    // 2. pwd   --> Print the Path of the Present working Directory
    // 3. cd    --> It will change the Directory.
    //--> In these we are Implemting the chdir() system call which will Changes the Current working Directory of the Calling Process to the Directory Specified in Path 


    // Implementing the exit
	char *path;
        int i, j;
    if(strncmp("exit",input_string,4) == 0)
    {
	exit(2);
    }

    // Implementing the pwd --> Print the Path of Present working Directory

    if(strncmp("pwd",input_string,3) == 0)
    {
	system("pwd");
    }


    // Implementing the cd ----> It will change the directory from current to the directory specified in the path

    if(strncmp("cd",input_string,2) == 0)		// checkinh the condition whether we have Entered cd command or  not in the command line
    {							// if you Entered 'cd' command means then

	int temp;
	
	//	chdir(&input_string[3]);			// chdir command it will change the path from current working directory to the directory specified in the path

	    for(i = 0 ; input_string[i]; i++)		// Running the for loop 
            {

            	if(input_string[i] == ' ' && input_string[i+1] == ' ')			// checking the condition whether the multiple spaces exist or not
            	{									// if Multiple spaces exists means then
                   for(j = i ; input_string[j]; j++)					// Running the for loop to remove/squeeze the spaces exist in the command_line argument i.e input_string
                   {
                 	input_string[j] = input_string[j + 1];
               	    }
               		 i--;
            	}
          }
       temp = chdir(&input_string[3]);			// After removing the multiple spaces from the input_string execute the chdir command	

       if(temp == -1)
       {
	   printf("Failed to change the Directory \n");

       }
    }
}
