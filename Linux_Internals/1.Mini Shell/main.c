/*
Name         : Shankar s
Date         : 24/05/23
Description  : P1 :- Minishell
*/

#include "main.h"

int main()
{
    system("clear"); 				// It will clear the  display screen / stdout whenever you runs/execute the project.

    // Array to store the Input
     char input_string[25];

     // Array to store the Prompt
     char prompt[25] = "msh";			// It is large enough to store, even you are customisising to New_prompt...eg :: PS1=minishell_project

     //scan the input
     scan_input(prompt,input_string);
     
     return  0;

}

