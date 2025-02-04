#include "inverted_index.h"

int save_DB(main_node_t **head, char *fname)
{
	//Open the file as write mode.
	FILE *fptr = fopen(fname, "w");

	//Running loop for storing the data in the file. 
	for (int i = 0; i < SIZE; i++)
	{
		//checking addresses is null or not.
		if (head[i])
		{
			//saving the data
			fprintf(fptr, "#%d;\n", i);

			//declare the main node 
			main_node_t *temp1 = head[i];

			//loop checking the temp1 
			while (temp1)
			{
				//Storing the data
				fprintf(fptr, "[%s];%d", temp1 -> word, temp1 -> f_count);

				//declare the sub node
				sub_node_t *temp = temp1 -> sub_link;

				//loop checking the temp
				while (temp)
				{
					//storing the data
					fprintf(fptr, ";%s;%d;", temp->f_name, temp->w_count);

					//replace the temp
					temp = temp -> link;
				}
				//storing the data
				fprintf(fptr, "#\n");
				//replace the temp1
				temp1 = temp1 -> link;
			}
		}
	}
	//Print the success message
	printf(CYAN"Successfull : Database saved in %s file\n",fname);

	//close the file pointer 
	fclose(fptr);
	return SUCCESS;
}


