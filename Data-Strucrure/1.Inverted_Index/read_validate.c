#include<stdlib.h>
#include "inverted_index.h"


//Validating input files given from Command line
int validate_n_store_filenames(file_node_t **head, char *filenames[])
{
	for(int i=1; filenames[i] != NULL; i++)
	{
		if(IsFileValid(filenames[i]) == SUCCESS)
		{
			if(store_filenames_to_list(filenames[i],head) == SUCCESS)
			{
				printf(CYAN"Successfull : %s Added to the list\n",filenames[i]);
			}
		}
	}
	return SUCCESS;
}

//checking whether the file is present or not and file is empty or not
int IsFileValid(char *file_name)
{
	FILE *fptr;
	fptr = fopen(file_name, "r");

	if(fptr == NULL)
	{
		printf(RED"Error : The %s is not present\n",file_name);
		printf("So we are not adding this file into the list\n");
		return NOT_PRESENT;
	}
	fseek(fptr,0,SEEK_END);

	if(ftell(fptr) >=1)
	{
		return SUCCESS;
	}
	else
	{
		printf(RED"Error : %s is an empty file\n",file_name);
		printf("So we are not adding this file into the list\n");
		return FILE_EMPTY;
	}

}

int store_filenames_to_list(char *f_name, file_node_t **head)
{
	file_node_t *new = malloc(sizeof(file_node_t));
	if(new == NULL)
		return FAILURE;
	strcpy(new->f_name, f_name);
	new->link = NULL;

	if(*head == NULL)
	{
		*head = new;
		return SUCCESS;
	}
	else
	{
		if (check_repeate(f_name, *head) == SUCCESS)
		{
			printf(RED"Error : The file %s is repeated\n",f_name);
			printf("So we are not adding this file into the list\n");
			return REPEATED;
		}
		else
		{
			file_node_t *temp = *head;

			while (temp->link != NULL)
				temp = temp->link;

			temp->link = new;
			return SUCCESS;
		}
	}
}
int check_repeate(char *f_name, file_node_t *head)
{
	while (head != NULL)
	{
		if (!strcmp(f_name, head->f_name))
			return SUCCESS;
		head = head->link;
	}
	return FAILURE;
}

int validation_store_filenames(file_node_t **file_head, char *filenames)
{
	int ret = IsFileValid(filenames);
	if(ret  == NOT_PRESENT)
	{


		return FAILURE;
	}
	else if(ret == FILE_EMPTY)
	{
	
	
		return FAILURE;
	}
	else
	{
		if(store_filenames_to_list(filenames, file_head) == REPEATED)
		{
	
			return FAILURE;
		}
		else
		{
			printf(CYAN"Successfully : %s Added to the list\n", filenames);
			return SUCCESS;
		}
	}
}


