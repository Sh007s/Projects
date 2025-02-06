#include "main.h"

// echo Function Definition , which will Implement echo Family Functions

void echo(char *input_string,int status)
{
    // In this we are goiing to Implement the following echo Family Funtions
    // 1. echo $? --> This Function will gives the Information regarding the execution of Previous command .  i.e It will give the exit status of Previous Command
    // 2. echo $$ --> It will gives the Process ID of the Minishell Project
    // 3. echo $SHELL --> It will print the Executable path of the minishell Project(Current Working Directory).

   
   // Implementing the ehco $? command

   if(strncmp("echo $?",input_string,7) == 0)
   {
       printf("%d \n",status);
   }

   // Implementing the echo $$ command

   if(strncmp("echo $$",input_string,7) == 0)
   {
       printf("%d \n",getpid());
   }

   // Implementing the echo $SHELL command

   if(strncmp("echo $SHELL",input_string,11) == 0)
   {
       system("pwd");
   }
}
