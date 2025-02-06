#include "main.h"

// Function definition for extract_external_commands 
void extract_external_commands(char ** external_commands)
{

    // variable declarations
    
   char ch;
   char buffer[25] = {'\0'};
   int i = 0, j=0;

   // open the file 
   int fd = open("external_commands.txt",O_RDONLY);

   if(fd == -1)
   {
       printf("Open Failed \n");
       exit(1);
   }

   // read the commands form the file 

   while(read (fd, &ch, 1) > 0)		// while loop it will run until it reaches to EOF 
   {					// read system call gives 0 when it reaches to the EOF
       if(ch != '\n')
       {
	   buffer[i++] = ch;
       }
       else
       {
	   // Allocate the sufficient Memory
	   external_commands[j] = calloc(strlen(buffer)+1,sizeof(char));

	   // copy the contents i.e commands from the external_commands.txt file to the array of pointers --> external_commands
	   strcpy(external_commands[j++],buffer);

	   // Append NULL character at the End
	   external_commands[j] = '\0';

	   // clear the Bufferr
	   memset(buffer, '\0', 25);
	    i= 0;
       }
   }
}

