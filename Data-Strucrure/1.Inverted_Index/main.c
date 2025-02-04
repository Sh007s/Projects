#include "inverted_index.h"

int main(int argc, char *argv[])                      //Function to read file names form CL
{
    int choice,flag = 0, index;
    char option;
    char word[BUFF_SIZE], file[BUFF_SIZE],backup[BUFF_SIZE];
    file_node_t *head = NULL;
    if(argc == 1)
    {
	printf(RED"Error : Invalid no.of argument\n");
	printf("Usage ./a.out < file.txt> <file1.txt> ...\n");
    }
    else
    {
	file_node_t *file_head = NULL;
	validate_n_store_filenames(&file_head, argv);
	if(file_head == NULL)
	{
	    printf(RED"There is no valid file\nPlase enter valid file\n");
	    return FAILURE;
	}
	main_node_t *head[SIZE] = {NULL};

	while(1)
	{
	    printf(ORANGE"1. Create Database\n2. Dispaly Database\n3. Search Database\n4. Updata Database\n5. Save Database\n");
	    printf(RED"Please Enter your choice : ");
	    printf(WHITE);
	    scanf("%d", &choice);

	    switch(choice)
	    {
		case 1:// case for create of data base
		    if(flag == 0)
		    {
			create_DB(file_head, head);
			flag = 1;
		    }
		    else
		    {
			printf(BLUE"Files already added to the data base\n");
		    }
		    break;
		case 2: // case for displaying of data base
		    display_DB(head);
		    break;
		case 3: // case for search a word in data base
		    printf(GREEN"Enter the word to be searched in Database : ");
		    printf(YELLOW);
		    scanf("%s", word);
		    index = tolower(word[0]) % 97;
		    if(index < 0 || index > 25)
			index = 26;
		    search_DB(head[index], word, index);
		    break;

		case 4: // case to update the file to the data base
		    printf(GREEN"Enter the filename : ");
		    scanf("%s", file);
		    update_DB(head, file_head, file);
		    break;
		case 5: // case to take a back up of data base and save data base
		    printf(GREEN"Enter the backup filename : ");
		    printf(YELLOW);
		    scanf("%s", backup);
		    save_DB(head, backup);
		    break;
		default:
		    printf(YELLOW"Invalid input\n");
		    break;
	    }
	    printf(BLUE"Want to continue press y or Y: ");
	    getchar();
	    scanf("%c", &option);
	    if(option != 'y' && option != 'Y')
		return SUCCESS;
	}
    }
}

