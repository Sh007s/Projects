#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"

//int dummysave = 0;

// Trim whitespace function
char *trim(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    *(end + 1) = '\0';

    return str;
}

Status load_file(AddressBookInfo *address_book)
{
    // Ensure address book is initialized
    if (address_book == NULL)
    {
        printf("Error: Address book is not initialized.\n");
        return e_failure;
    }

    // Allocate memory for the contact list if not already allocated
    if (address_book->list == NULL)
    {
        address_book->list = malloc(sizeof(ContactInfo) * MAX_CONTACTS);
        if (address_book->list == NULL)
        {
            printf("Error: Memory allocation failed.\n");
            return e_failure;
        }
    }

    // Use the Read_From_CSV function to load data
    if (Read_From_CSV(address_book, address_book->default_name) == e_failure)
    {
        printf("Failed to load data from the file.\n");
        return e_failure;
    }

    // Display the loaded contacts
    if (address_book->count > 0)
    {
        printf("\nTotal contacts loaded: %d\n", address_book->count);
    //    Display_Contacts(address_book);
    }
    else
    {
  //      printf("\nNo contacts loaded. The address book is empty.\n");
    }

    return e_success;
}

Status Read_From_CSV(AddressBookInfo *address_book, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file %s\n", filename);
        return e_failure;
    }

    // Skip header line
    char header[MAX_STRING_LENGTH];
    fgets(header, sizeof(header), file);

    char line[MAX_STRING_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        if (address_book->count >= MAX_CONTACTS)
        {
            printf("Error: Maximum contact limit reached.\n");
            break;
        }

        ContactInfo *contact = &address_book->list[address_book->count];

      //  printf("Processing line: '%s'\n", line);

        // Parse Serial_No
        char *token = strtok(line, ",");
   //     printf("Serial NUmber %s\n", token);
        if (token)
        {
            token = trim(token);
     //       printf("Raw Serial Token: '%s'\n", token); // Debugging the token
            contact->Serial_No = atoi(token);
     //       printf("Parsed Serial Number: %d\n", contact->Serial_No); // Debugging the result
        }
        else
        {
            printf("Skipping incomplete contact: Missing Serial_No.\n");
            continue;
        }

        // Parse Name
        token = strtok(NULL, ",");
        if (token)
        {
            strncpy(contact->name, token, NAME_LEN - 1);
            contact->name[NAME_LEN - 1] = '\0'; // Ensure null termination
        }
        else
        {
            printf("Skipping incomplete contact: Missing Name.\n");
            continue;
        }

        // Parse Phone Numbers
        token = strtok(NULL, ",");
        if (token)
        {
            contact->phone_count = 0;
            char *start = token;
            char *end;

            while ((end = strstr(start, ";")) && contact->phone_count < MAX_PHONE_NUMBERS)
            {
                *end = '\0'; // Null-terminate the current token

                // Validate and copy phone numbers
                if (is_valid_phone_number(start))
                {
                    strncpy(contact->phone_number[contact->phone_count], start, PHONE_LEN - 1);
                    contact->phone_number[contact->phone_count][PHONE_LEN - 1] = '\0'; // Ensure null termination
                    contact->phone_count++;
                }

                start = end + 1; // Move to the next token after the delimiter
            }

            // Process the last token (if any) after the final semicolon
            if (*start != '\0' && contact->phone_count < MAX_PHONE_NUMBERS)
            {
                if (is_valid_phone_number(start))
                {
                    strncpy(contact->phone_number[contact->phone_count], start, PHONE_LEN - 1);
                    contact->phone_number[contact->phone_count][PHONE_LEN - 1] = '\0'; // Ensure null termination
                    contact->phone_count++;
                }
            }
        }

        token = strtok(NULL, "\n");
        if (token)
        {
            // Preserve the original Serial_No before processing emails
            int original_serial_no = contact->Serial_No;

            contact->email_count = 0; // Reset email count for each contact

            char *email_token = strtok(token, ";");

            while (email_token && contact->email_count < MAX_EMAIL_IDS)
            {
                // Trim the email token
                email_token = trim(email_token);

                // Process email if valid
                if (strlen(email_token) > 0)
                {
                    // Ensure we don't exceed the array bounds
                    strncpy(contact->email_addresses[contact->email_count],
                            email_token,
                            EMAIL_LEN - 1);
                    contact->email_addresses[contact->email_count][EMAIL_LEN - 1] = '\0';

                    contact->email_count++; // Increment email count AFTER adding email
                }

                email_token = strtok(NULL, ";"); // Get next email token
            }

            // Restore the original Serial_No after processing emails
            contact->Serial_No = original_serial_no;
        }
        address_book->count++;
    }

    fclose(file);
    printf("Data loaded successfully from '%s'.\n", filename);
    return e_success;
}

Status Display_Contacts(AddressBookInfo *address_book)
{
    printf("Address Book:\n");
    for (int i = 0; i < address_book->count; i++)
    {
        ContactInfo *contact = &address_book->list[i];

        printf("Serial No: %d\n", contact->Serial_No);
        printf("Name: %s\n", contact->name);

        printf("Phone Numbers:\n");
        for (int j = 0; j < contact->phone_count; j++)
        {
            printf("  %d: %s\n", j + 1, contact->phone_number[j]);
        }

        printf("Email Addresses:\n");
        for (int j = 0; j < contact->email_count; j++)
        {
            printf("  %d: %s\n", j + 1, contact->email_addresses[j]);
        }
        printf("\n");
    }
    return e_success;
}

Status Save_File(AddressBookInfo *addressbook)
{
     if (addressbook->count == 0)
    {
        printf("No contacts available to save.\n");
        return e_failure;
    }

    // Check if the file pointer is NULL
    if (addressbook->fp == NULL)
    {
        // Open the file in write mode if not already opened
        addressbook->fp = fopen(addressbook->default_name, "w");
        if (addressbook->fp == NULL)
        {
            printf("Error: Failed to open file '%s' for saving.\n", addressbook->default_name);
            return e_failure;
        }
    }

    // Write the header row
    fprintf(addressbook->fp, "Serial No,Name,Phone Numbers,Email Addresses\n");

    // Write each contact's details
    for (int i = 0; i < addressbook->count; i++)
    {
        ContactInfo *contact = &addressbook->list[i];

        // Write Serial No and Name
        fprintf(addressbook->fp, "%d,%s,", contact->Serial_No, contact->name[0] != '\0' ? contact->name : "N/A");

        // Write phone numbers, separated by semicolons
        for (int j = 0; j < MAX_PHONE_NUMBERS; j++)
        {
            if (contact->phone_number[j][0] != '\0')
            {
                fprintf(addressbook->fp, "%s", contact->phone_number[j]);
                if (j < MAX_PHONE_NUMBERS - 1 && contact->phone_number[j + 1][0] != '\0')
                {
                    fprintf(addressbook->fp, ";");
                }
            }
        }
        fprintf(addressbook->fp, ",");

        // Write email addresses, separated by semicolons
        for (int k = 0; k < MAX_EMAIL_IDS; k++)
        {
            if (contact->email_addresses[k][0] != '\0')
            {
                fprintf(addressbook->fp, "%s", contact->email_addresses[k]);
                if (k < MAX_EMAIL_IDS - 1 && contact->email_addresses[k + 1][0] != '\0')
                {
                    fprintf(addressbook->fp, ";");
                }
            }
        }
        fprintf(addressbook->fp, "\n");
    }
    fflush(addressbook->fp); // Ensure everything is written to disk
 //   fclose(addressbook->fp);
    printf("File saved successfully with %d contact(s).\n", addressbook->count);
    dummysave = 1;
    return e_success;
}

