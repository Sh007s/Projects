#include "inverted_index.h"
/*
 * Function defination 
 * To display the Database
 */
int display_DB(main_node_t **head)
{
	//Running loop printing upto SIZE
	for (int i = 0; i < SIZE; i++)
	{
		//Check that address is null or not
		if (head[i])
			printf(YELLOW"[%d]", i);

		//declare a temperary main node
		main_node_t *temp1 = head[i];

		//Loop for checking temp and print the database
		while (temp1)
		{
			//print the content
			printf("\t"GREEN"[%s]\t"RED"%d  "WHITE"file(s) : File : ",temp1 -> word, temp1 -> f_count);

			//declare a temperary sub node
			sub_node_t *temp = temp1 -> sub_link;

			//Loop for checking sub node and print the content
			while (temp)
			{
				printf(""GREEN"%s\t"RED"%d "WHITE"time(s) -> ", temp->f_name, temp->w_count);

				//replace the sub node
				temp = temp -> link;
			}
			printf(WHITE"NULL\n");

			//replace the main node
			temp1 = temp1 -> link;
		}
	}
	return SUCCESS;
}
