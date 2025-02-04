#include "inverted_index.h"

//Funtion to search a word from a data base
int search_DB(main_node_t *head, char *word, int index)
{
	main_node_t *main_temp = head;

	while (main_temp != NULL)
	{
		//stcmp function to search given word with data base
		if (strcmp(main_temp->word, word) == 0)
		{
			//if the word is present it will print this message
			printf(RED"Word "GREEN"%s "RED"found in the Database and it present in "GREEN"%d "RED"file(s)\n", word, main_temp -> f_count);

			sub_node_t *sub_temp = main_temp -> sub_link;

			//Updating the sub node
			while (sub_temp != NULL)
			{
				printf(RED"In file "GREEN"%s "GREEN"%d "RED"time(s)\n", sub_temp -> f_name, sub_temp -> w_count);
				sub_temp = sub_temp -> link;
			}
			return SUCCESS;
		}
		else
			main_temp = main_temp -> link;
	}
	printf(RED"Error : Word %s not found in the Database\n", word);
	return FAILURE;
}

