#include "main.h"

// Function definition for scan_input
void scan_input(char *prompt, char *input_string)
{
    
    char *command = '\0';			//variable declaration 
    
    int command_type,status;
    pid_t pid;

    while (1)
    {
	printf(ANSI_COLOR_GREEN"[%s]$ " ANSI_COLOR_RESET,prompt);	// printing the prompt in RED color

	//clear the output buffer by calling fflush ,if you dont do these sometimes there is a chance of  ur output and input buffer get effected so, clear the output and input buffer

	fflush(stdout);
	scanf("%[^\n]s",input_string);

	//clear the input buffer
	getchar();

	//customise the Prompt	by using the Environmental variable : PS1	eg ::: PS1=msh

	if(!strncmp("PS1=",input_string,4))			// checking whether the first 4 characters of the input_string contains PS1= or not 
	{						        // if it contains means then 
	    // customise the Prompt
	    if(input_string[4] != ' ')				// check the 5th character is not equal to ' ' (white space)
	      {							// if it is not a white space means then only
		strcpy(prompt,&input_string[4]);  		//  copy the new_prompt to Prompt pointer from 5th index of the input_string

		// clear the input 
		memset(input_string,'\0',25);
		continue;
	      }
	    printf("Command not found\n");
	}
	else
	{
            printf("Command not found\n");
            exit(1);
        }
	    // To get Command
	    command = get_command(input_string);
//	    printf("Command is %s \n",command);			// printing the command 
	    // check whether the command you entered/typed is Internal or External command by calling the check_command_type function

	    command_type = check_command_type(command);		// calling the check_command_type function , these function will return the value based on the type of command for eg: Internal it will return 1,for External it will return 2, for NO_COMMAND it will return 3

//	    printf("command_type is %d\n",command_type);

	    //Implement External commands

	    if(command_type == EXTERNAL)
	    {
		/* create a child process and execute the command
		 * Parent should wait for the child to complte
		 * Only on completion , minishell prompt should displayed 
		 */

		  pid = fork();			// calling the fork system call it will create a child process 
						// on, SUCCESS it will return the PID of the child process in parent Process
						// on, SUCCESS it will return the 0 in the child process

		if(pid == -1)			// checking the condition whether the pid value is -1	
		{				// if it is -1 means that Fork system call is failed
		    printf("Fork is Failed \n");
		    exit(1);
		}
		else if (pid == 0)		// checking the condition whether the pid value is equal to 0 or not
		{				// if it is equal to 0 means that is CHILD process

		    // child process
		    int ret_status = system(input_string);	// calling the system Function
	            // System is a Library Function it will execute the shell commands
	            // By this we can execute a commands through program	    

		    if(ret_status == 0)				// on sucessfull execution of the command  it will return the 0 i.e->it will give the exit status of that command
		    {
			exit(0);
		    }
		}

		else
		{
		    // parent process

		    wait(&status);				// calling the wait function it will block the parent process untill it's child completes it's execution
		    if(WIFEXITED(status))
		    {
			printf("Child process terminated %d \n",pid);	
		    }
		}
	    } 

	    // Implement the Family of echo Functions
	    echo(input_string,status);			// calling the echo function which will implement echo Commands

	    // Implement Internal command
	    execute_internal_commands(input_string);	    
    }
}
