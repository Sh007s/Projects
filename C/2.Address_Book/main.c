#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "file_ops.h"

int isSave = 0;
// int dummysave = 0;

int main(int argc, char *argv[])
{
	AddressBookInfo addressbook;
	int ret;

	// Initialize address book
	addressbook.count = 0;
	addressbook.fp = NULL;
	addressbook.list = malloc(sizeof(ContactInfo) * MAX_CONTACTS);
	if (!addressbook.list)
	{
		printf("Memory allocation failed for contacts list.\n");
		return e_failure;
	}

	if (argc == 1)
	{
		printf("Only ./a.out is passed\n");
		addressbook.default_name = DEFAULT_NAME;
		if (DummyContact(&addressbook) == e_success)
		{
			printf("Dummy contacts created successfully!\n");
			printf("In \"%s\" Default CSV File.\n", addressbook.default_name);
		}
		else
		{
			printf("Failed to created the Dummyc contact\n");
			return e_failure;
		}
	}

	else if (argc == 2)
	{

		//		printf("argv[2] is passed %s\n",argv[1]);
		if (strstr(argv[1], ".csv") == NULL)
		{
			printf("%s is not .csv file\n", argv[1]);
			printf("Usage : ./a.out address_book.csv\n");
			return e_failure;
		}

		// Assign the filename to addressbook.default_name
		addressbook.default_name = argv[1];

		ret = load_file(&addressbook);
		if (ret != e_success)
		{
			printf("Error: Unable to load or create the default file.\n");
			return e_failure;
		}
	}

	else
	{
		printf("Invalid arguments.\n");
		printf("Usage:  ./address_book.csv\n");
		return e_failure;
	}

	int option;
	do
	{
		option = menu();

		if (option == e_invalid)
		{
			printf("Invalid input. Please enter a number between 0 and 6.\n");
			continue;
		}

		switch (option)
		{
		case 0:
		{
			int result = exit_menu(&addressbook);
			if (result == e_success)
			{
				printf("Exiting. Data saved in address_book.csv\n");
				isSave = 0;
				dummysave = 0;
			}
			else if (result == e_failure)
			{
				printf("Exiting. No changes saved.\n");
			}
			else if (result == e_invalid)
			{
				printf("Invalid option. Please enter 'N' or 'Y'.\n");
			}
			break;
		}
		case 1:
			if (Add_Contact(&addressbook) == e_success)
			{
				printf("Contact saved successfully.\n");
			}
			break;
		case 2:
			if (Search_Contact(&addressbook) == e_success)
			{
				printf("Contact search completed successfully.\n");
			}
			break;
		case 3:
			if (Edit_Contact(&addressbook) == e_success)
			{
				printf("Contact edited successfully.\n");
			}
			else
			{
				printf("Failed to edit contact.\n");
			}
			break;
		case 4:
			if (Delete_Contact(&addressbook) == e_success)
			{
				printf("Contact deleted successfully.\n");
			}
			break;
		case 5:
			if (List_Contact(&addressbook) == e_success)
			{
				printf("Contact list displayed successfully.\n");
			}
			break;
		case 6:
			if (Save_File(&addressbook) == e_success)
			{
				printf("File saved successfully.\n");
				isSave = 1;
				dummysave = 1;
			}
			break;
		default:
			printf("Invalid option. Please try again.\n");
		}
	} while (option != 0);
	return e_success;
}
