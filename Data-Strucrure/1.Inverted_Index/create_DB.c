#include"inverted_index.h"

//Funtion to create Data Base
int create_DB(file_node_t *file_head, main_node_t **head)
{
	while (file_head != NULL)
	{
		read_datafile(head, file_head -> f_name);
		file_head = file_head -> link;
	}
	return SUCCESS;
}

//Funtion to read data form the file and find the index of each word in the file and store the file in subnode
void read_datafile(main_node_t **head, char *f_name)
{
	FILE *fptr = fopen(f_name, "r");
	char word[BUFF_SIZE];
	short index, flag = 0;
	while (fscanf(fptr, "%s", word) != EOF)
	{

		index = tolower(word[0]) % 97;	
		if (index > 25 || index < 0)
			index = 26;

		if (head[index] == NULL)
			insert_at_last_main(&head[index], word, f_name);
		else
		{
			main_node_t *temp = head[index];
			while(temp)
			{
				if (strcmp(temp->word, word)==0)
				{
					update_word_count(&temp, f_name);
					flag = 1;
					break;
				}
				else
					temp =temp ->link;
			}
			if(flag == 0)
				insert_at_last_main(&head[index], word, f_name);
		}
	}
	printf(CYAN"Successfull: Creation of database for file %s\n", f_name);
}


// Function to insert at last
int insert_at_last_main(main_node_t **head, char *word, char *f_name)
{
	main_node_t *new_main = malloc(sizeof(main_node_t));
	if (new_main ==NULL)
		return FAILURE;

	new_main -> f_count = 1;
	strcpy(new_main -> word, word);
	new_main -> link = NULL;
	update_subnode(&new_main, f_name);

	if (*head == NULL)
		*head = new_main;
	else
	{
		main_node_t *temp = *head;
		while (temp->link)
			temp = temp->link;
		temp -> link = new_main;
	}
	return SUCCESS;
}


//Funtion to update subnod
int update_subnode(main_node_t **main_node, char *f_name)
{
	sub_node_t *new_sub = malloc(sizeof(sub_node_t));
	if (new_sub == NULL)
		return FAILURE;

	new_sub -> w_count = 1;
	strcpy(new_sub -> f_name, f_name);
	new_sub -> link = NULL;

	(*main_node)->sub_link = new_sub;

	return SUCCESS;
}


//funtion to update word count of of subnod
int update_word_count(main_node_t **head, char *f_name)
{
	sub_node_t *temp = (*head) -> sub_link, *prev = NULL;

	while(temp)
	{
		if (!strcmp(f_name, temp -> f_name))
		{
			temp -> w_count += 1;
			return SUCCESS;
		}
		else
		{
			prev = temp;
			temp = temp -> link;
		}
	}
	(*head)->f_count += 1; 
	sub_node_t *new_sub = malloc(sizeof(sub_node_t));
	if (new_sub == NULL)
		return FAILURE;

	new_sub -> w_count = 1;
	strcpy(new_sub -> f_name, f_name);
	new_sub -> link = NULL;

	prev -> link = new_sub;
	return SUCCESS;
}
