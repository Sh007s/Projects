#include <stdio.h>
#include "main.h"
#include "file_ops.h"

int dummysave = 0;

int menu()
{
    int op;
    printf("########  Address Book ########\n");
    printf("########  Feature: \n");

    printf("0.Exit \n1.Add Contact \n2.Search Contact \n3.Edit Contact \n4.Delete Contact \n5.List Contact \n6.Save File\n");
    printf("Plase Select an option : ");

    if (scanf("%d", &op) != 1)
    {
        while (getchar() != '\n')
            ;
        return e_invalid;
    }
    return op;
}

Status Dummy_Contact_info(AddressBookInfo *addressbook)
{
    Dummy_Contact_info(addressbook);
    printf("Passing the Dummy Contact\n");
    if (DummyContact(addressbook) == e_success)
    {
        for (int i = 0; i < addressbook->count; i++)
        {
            printf("Serial No: %d\n", addressbook->list[i].Serial_No);
            printf("Name: %s\n", addressbook->list[i].name);

            printf("Phone Numbers:\n");
            for (int j = 0; j < addressbook->list[i].phone_count; j++)
            {
                printf("  %s\n", addressbook->list[i].phone_number[j]);
            }

            printf("Email Addresses:\n");
            for (int j = 0; j < addressbook->list[i].email_count; j++)
            {
                printf("  %s\n", addressbook->list[i].email_addresses[j]);
            }
            printf("\n");
        }

        // Free allocated memory
        free(addressbook->list);
    }
    else
    {
        printf("Failed to create dummy contacts.\n");
    }
    return e_success;
}

Status DummyContact(AddressBookInfo *addressbook)
{
    if (addressbook == NULL)
    {
        printf("DEBUG: AddressBook pointer is not created or initialized.\n");
        return e_invalid; // Return error if the address book pointer is NULL
    }

    addressbook->count = 3;
    addressbook->list = (ContactInfo *)malloc(sizeof(ContactInfo) * addressbook->count);
    if (addressbook->list == NULL)
    {
        printf("ERROR: Memory allocation for addressbook->list failed.\n");
        return e_failure;
    }

    // Populate dummy data
    for (int i = 0; i < addressbook->count; i++)
    {
        snprintf(addressbook->list[i].name, NAME_LEN, "Contact%d", i + 1);
        addressbook->list[i].Serial_No = i + 1;

        addressbook->list[i].phone_count = 2; // Example: Each contact has 2 phone numbers
        for (int j = 0; j < addressbook->list[i].phone_count; j++)
        {
            snprintf(addressbook->list[i].phone_number[j], NUMBER_LEN, "123456789%d", j + i);
        }

        addressbook->list[i].email_count = 2; // Example: Each contact has 2 email addresses
        for (int j = 0; j < addressbook->list[i].email_count; j++)
        {
            snprintf(addressbook->list[i].email_addresses[j], EMAIL_ID_LEN, "contact%d_%d@example.com", i + 1, j + 1);
        }
    }
    // int result = Save_File(addressbook);
    // if (result == e_success)
    // {
    //     printf("Dunny contact saved\n");
    // }
    return e_success;
}

int is_valid_phone_number(const char *phone)
{
    int i = 0;

    // Handle empty string or NULL
    if (!phone || !phone[0])
        return 0;

    // Optional '+' prefix
    if (phone[i] == '+')
        i++;

    // Need at least one digit after '+'
    if (!phone[i])
        return 0;

    // Check remaining characters are all digits
    int digit_count = 0;
    for (; phone[i]; i++)
    {
        if (!isdigit(phone[i]))
            return 0;
        digit_count++;
    }

    // Check length requirements (7-15 digits)
    return (digit_count >= 7 && digit_count <= 15);
}

int is_valid_email(const char *email)
{
    const char *at = strchr(email, '@');
    const char *dot = strchr(email, '.');

    return at && dot && at < dot;
}

int search_contacts(AddressBookInfo *addressbook, ContactInfo *temp, ContactInfo *results, int max_results)
{
    int result_count = 0;

    for (int i = 0; i < addressbook->count; i++)
    {
        ContactInfo *contact = &addressbook->list[i];
        int match_found = 0;

        // Check for name match (case-insensitive)
        if (strlen(temp->name) > 0 && strcasecmp(contact->name, temp->name) == 0)
        {
            match_found = 1;
        }

        // Check for phone number match
        if (!match_found && strlen(temp->phone_number[0]) > 0)
        {
            for (int j = 0; j < MAX_PHONE_NUMBERS; j++)
            {
                if (strlen(contact->phone_number[j]) > 0 && strcmp(contact->phone_number[j], temp->phone_number[0]) == 0)
                {
                    match_found = 1;
                    break;
                }
            }
        }

        // Check for email address match
        if (!match_found && strlen(temp->email_addresses[0]) > 0)
        {
            for (int j = 0; j < MAX_EMAIL_IDS; j++)
            {
                if (strlen(contact->email_addresses[j]) > 0 && strcasecmp(contact->email_addresses[j], temp->email_addresses[0]) == 0)
                {
                    match_found = 1;
                    break;
                }
            }
        }

        // Check for Serial_No match
        if (!match_found && temp->Serial_No >= 0 && contact->Serial_No == temp->Serial_No)
        {
            match_found = 1;
        }

        // Add to results if there's a match
        if (match_found)
        {
            if (result_count < max_results)
            {
                results[result_count++] = *contact;
            }
            else
            {
                printf("Warning: Maximum result limit reached.\n");
                break;
            }
        }
    }

    return result_count;
}

int case_insensitive_compare(const char *str1, const char *str2)
{
    // Convert both strings to lowercase before comparing
    while (*str1 && *str2)
    {
        if (tolower(*str1) != tolower(*str2))
        {
            return 0; // Not a match
        }
        str1++;
        str2++;
    }
    return *str1 == *str2; // Check for string length match
}

void display_results(ContactInfo *results, int count, ContactInfo *temp)
{
    ContactInfo filtered_results[count]; // Temporary array to store filtered results
    int filtered_count = 0;

    // Filter results based on search criteria in `temp`
    for (int i = 0; i < count; i++)
    {
        int match = 1; // Assume match until proven otherwise
        if (strlen(temp->name) > 0 && strcmp(results[i].name, temp->name) != 0)
        {
            continue; // Skip non-matching names
        }

        // Check phone number match
        if (match && strlen(temp->phone_number[0]) > 0)
        {
            int phone_match = 0;
            for (int j = 0; j < MAX_PHONE_NUMBERS; j++)
            {
                if (strlen(results[i].phone_number[j]) > 0 &&
                    strcmp(results[i].phone_number[j], temp->phone_number[0]) == 0)
                {
                    phone_match = 1;
                    printf("Phone number match found\n");
                    break;
                }
            }
            if (!phone_match)
            {
                match = 0;
                printf("Phone number does not match\n");
            }
        }

        // Check email address match (case-insensitive)
        if (match && strlen(temp->email_addresses[0]) > 0)
        {
            int email_match = 0;
            for (int j = 0; j < MAX_EMAIL_IDS; j++)
            {
                if (strlen(results[i].email_addresses[j]) > 0 &&
                    strcasecmp(results[i].email_addresses[j], temp->email_addresses[0]) == 0)
                {
                    email_match = 1;
                    printf("Email match found\n");
                    break;
                }
            }
            if (!email_match)
            {
                match = 0;
                printf("Email does not match\n");
            }
        }

        // Check Serial_No match
        if (match && temp->Serial_No > 0 && results[i].Serial_No != temp->Serial_No)
        {
            match = 0;
            printf("Serial No does not match\n");
        }

        // Add matching result to the filtered list
        if (match)
        {
            filtered_results[filtered_count++] = results[i];
            printf("Contact added to filtered results\n");
        }
    }

    // Display filtered results
    if (filtered_count == 0)
    {
        printf("No matching contacts found.\n");
        return;
    }

    char quit;
    do
    {
        printf("\n#### Search Results ####\n");
        printf("============================================================\n");
        printf("%-5s %-30s %-15s %-40s\n", "S.No", "Name", "Phone No", "Email ID");
        printf("============================================================\n");

        for (int i = 0; i < filtered_count; i++)
        {
            // Print the first phone number and email ID on the same line as Name
            printf("%-5d %-30s %-15s %-40s\n",
                   filtered_results[i].Serial_No,
                   filtered_results[i].name,
                   filtered_results[i].phone_number[0],
                   filtered_results[i].email_addresses[0]);

            // Print remaining phone numbers and email IDs
            int max_rows = (MAX_PHONE_NUMBERS > MAX_EMAIL_IDS) ? MAX_PHONE_NUMBERS : MAX_EMAIL_IDS;

            for (int j = 1; j < max_rows; j++)
            {
                printf("%-5s %-30s",
                       "", // Empty S.No and Name columns for subsequent rows
                       "");

                if (j < MAX_PHONE_NUMBERS && strlen(filtered_results[i].phone_number[j]) > 0)
                {
                    printf("%-15s", filtered_results[i].phone_number[j]);
                }
                else
                {
                    printf("%-15s", ""); // Leave space if no phone number
                }

                if (j < MAX_EMAIL_IDS && strlen(filtered_results[i].email_addresses[j]) > 0)
                {
                    printf("%-40s", filtered_results[i].email_addresses[j]);
                }
                else
                {
                    printf("%-40s", ""); // Leave space if no email address
                }

                printf("\n"); // Move to the next line
            }
        }

        printf("============================================================\n");
        printf("Press [q] to Cancel: ");

        // Read quit character
        if (scanf(" %c", &quit) != 1)
        {
            printf("Invalid input. Defaulting to quit.\n");
            quit = 'q';
        }

        // Clear input buffer
        while (getchar() != '\n')
            ;
    } while (quit != 'q' && quit != 'Q');

    // Clear the search criteria when 'q' is pressed
    if (quit == 'q' || quit == 'Q')
    {
        memset(temp->name, 0, sizeof(temp->name));
        memset(temp->phone_number, 0, sizeof(temp->phone_number));
        memset(temp->email_addresses, 0, sizeof(temp->email_addresses));
        temp->Serial_No = -1; // Reset Serial_No
        printf("Search criteria cleared.\n");
    }
}

int Perform_Search(AddressBookInfo *addressbook, ContactInfo *criteria, ContactInfo results[], int max_results)
{
    int result_count = search_contacts(addressbook, criteria, results, max_results);
    if (result_count > 0)
    {
        for (int i = 0; i < result_count; i++)
        {
            if (case_insensitive_compare(results[i].name, criteria->name))
            {
                // Print search result
                // Continue processing results
            }
        }
    }
    else
    {
        printf("No matching contacts found.\n");
    }
    return result_count;
}

Status Search_Menu(ContactInfo *criteria)
{
    int op;
    do
    {
        printf("\nSearch Menu:\n");
        printf("0. Back\n");
        printf("1. Enter the Name\n");
        printf("2. Enter the Phone No\n");
        printf("3. Enter the Email ID\n");
        printf("4. Enter the Serial No\n");
        printf("Please select an option: ");

        if (scanf("%d", &op) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        while (getchar() != '\n')
            ; // Clear input buffer

        switch (op)
        {
        case 0:
            return 0;
        case 1:
            printf("Enter Name: ");
            fgets(criteria->name, sizeof(criteria->name), stdin);
            criteria->name[strcspn(criteria->name, "\n")] = '\0';
            break;
        case 2:
            printf("Enter Phone Number: ");
            fgets(criteria->phone_number[0], NUMBER_LEN, stdin);
            criteria->phone_number[0][strcspn(criteria->phone_number[0], "\n")] = '\0';
            break;
        case 3:
            printf("Enter Email ID: ");
            fgets(criteria->email_addresses[0], EMAIL_ID_LEN, stdin);
            criteria->email_addresses[0][strcspn(criteria->email_addresses[0], "\n")] = '\0';
            break;
        case 4:
            printf("Enter Serial No: ");
            if (scanf("%d", &criteria->Serial_No) != 1)
            {
                printf("Invalid input. Please enter a valid Serial No.\n");
                while (getchar() != '\n')
                    ;
                criteria->Serial_No = -1;
            }
            break;
        default:
            printf("Invalid option. Try again.\n");
        }
        return 1; // Return non-zero if valid search criteria were entered
    } while (op != 0);
    return e_success;
}

// Add Contact function
Status Add_Contact(AddressBookInfo *addressbook)
{
    ContactInfo currentContact = {0};
    currentContact.Serial_No = addressbook->count + 1;
    printf("Serial No Count : %d\n", currentContact.Serial_No);
    int op;
    int contactAdded = 0; // Flag to track if the contact is added successfully

    do
    {
        printf("Add Contact Menu\n");
        printf("\n0. Back\n1. Name       : %s\n2. Phone No   : %s\n3. Email ID   : %s\n",
               (currentContact.name[0] != '\0') ? currentContact.name : "N/A",
               (currentContact.phone_number[0][0] != '\0') ? currentContact.phone_number[0] : "N/A",
               (currentContact.email_addresses[0][0] != '\0') ? currentContact.email_addresses[0] : "N/A");

        printf("Please select an option: ");

        // Handle invalid input in a loop
        while (scanf("%d", &op) != 1)
        {
            printf("Invalid input, please enter a number.\n");
            while (getchar() != '\n') // Clear input buffer
                ;
            printf("Please select an option: ");
        }

        while (getchar() != '\n')
            ;

        // Handle menu options
        switch (op)
        {
        case 0: // Exit the menu
            printf("Exiting Contact Menu.\n");
            break;

        case 1: // Enter Name
            printf("Enter the name for Contact %d: ", addressbook->count + 1);
            fgets(currentContact.name, sizeof(currentContact.name), stdin);
            currentContact.name[strcspn(currentContact.name, "\n")] = '\0'; // Remove newline

            // Check if the name is empty
            if (strlen(currentContact.name) == 0)
            {
                printf("Invalid Name, Name cannot be empty.\n");
            }
            else
            {
                contactAdded = 1;
            }
            break;

        case 2: // Enter Phone Number
            printf("Enter the phone number for Contact %d: ", addressbook->count + 1);
            fgets(currentContact.phone_number[0], sizeof(currentContact.phone_number[0]), stdin); // Fixed to use phone_number[0]
            currentContact.phone_number[0][strcspn(currentContact.phone_number[0], "\n")] = '\0'; // Fixed to use phone_number[0]
            if (!is_valid_phone_number(currentContact.phone_number[0]))
            { // Fixed to use phone_number[0]
                printf("Invalid Phone Number. Please enter a valid phone number.\n");
                memset(currentContact.phone_number[0], 0, sizeof(currentContact.phone_number[0])); // Fixed to use phone_number[0]
            }
            else
            {
                contactAdded = 1;
            }
            break;

        case 3: // Enter Email ID
            printf("Enter the email ID for Contact %d: ", addressbook->count + 1);
            fgets(currentContact.email_addresses[0], sizeof(currentContact.email_addresses[0]), stdin); // Fixed to use email_addresses[0]
            currentContact.email_addresses[0][strcspn(currentContact.email_addresses[0], "\n")] = '\0'; // Fixed to use email_addresses[0]
            if (!is_valid_email(currentContact.email_addresses[0]))
            { // Fixed to use email_addresses[0]
                printf("Invalid Email ID. Please enter a valid email.\n");
                memset(currentContact.email_addresses[0], 0, sizeof(currentContact.email_addresses[0])); // Fixed to use email_addresses[0]
            }
            else
            {
                contactAdded = 1;
            }
            break;

        default: // Invalid option
            printf("Please select a valid option.\n");
            break;
        }

        // Only save the contact if all fields are filled and valid
        if (contactAdded && strlen(currentContact.name) > 0 && strlen(currentContact.phone_number[0]) > 0 && strlen(currentContact.email_addresses[0]) > 0)
        {
            // **Duplicate Check**
            int duplicate = 0;
            for (int i = 0; i < addressbook->count; i++)
            {
                if (strcasecmp(addressbook->list[i].name, currentContact.name) == 0 &&
                    strcasecmp(addressbook->list[i].phone_number[0], currentContact.phone_number[0]) == 0)
                {
                    printf("Contact already exists.\n");
                    duplicate = 1;
                    break;
                }
            }

            // Only add if no duplicate was found
            if (!duplicate)
            {
                // Allocate memory for the new contact
                ContactInfo *newList = realloc(addressbook->list, (addressbook->count + 1) * sizeof(ContactInfo));
                if (newList == NULL)
                {
                    printf("Memory allocation failed.\n");
                    return e_failure;
                }
                addressbook->list = newList;

                // Save the current contact
                addressbook->list[addressbook->count] = currentContact;
                addressbook->count++;
                contactAdded = 0; // Reset flag after adding the contact

                // Reset currentContact for the next entry
                currentContact = (ContactInfo){0};
                currentContact.Serial_No = addressbook->count + 1;

                printf("Contact saved successfully.\n");
            }
        }

    } while (op != 0); // End the loop when user chooses option 0

    return e_success;
}

// Search Contact function
Status Search_Contact(AddressBookInfo *addressbook)
{
    ContactInfo criteria = {0}; // Initialize search criteria
    criteria.Serial_No = 1;
    ContactInfo results[MAX_RESULTS];

    while (Search_Menu(&criteria))
    {
        int result_count = Perform_Search(addressbook, &criteria, results, MAX_RESULTS);
        if (result_count > 0)
        {
            display_results(results, result_count, &criteria); // Show results if any
        }
        memset(&criteria, 0, sizeof(criteria)); // Clear criteria for the next search
    }

    return e_success;
}

int quit_option()
{
    char quit;
    while (1)
    {
        printf("Enter Quit to [q], Select to [s]: ");
        scanf(" %c", &quit); // Note the leading space to handle any leftover whitespace
        if (quit == 'q' || quit == 'Q')
        {
            return 0; // Quit selected
        }
        else if (quit == 's' || quit == 'S')
        {
            return 1; // Continue selected
        }
        else
        {
            printf("Invalid input. Please enter 'q' to quit or 's' to select.\n");
        }
    }
}

Status Edit_Contact(AddressBookInfo *addressbook)
{
    if (addressbook->count == 0)
    {
        printf("No contacts available to edit.\n");
        return e_invalid;
    }

    // Display all contacts to select from
    printf("Select contact to edit:\n");
    for (int i = 0; i < addressbook->count; i++)
    {
        printf("%d. %s\n", i + 1, addressbook->list[i].name);
    }

    // Get user choice
    int choice = quit_option();
    if (choice == 0)
    {
        printf("Edit operation canceled.\n");
        return e_success;
    }
    int choice_select;
    printf("Enter the Contact Serial No to Edit (1 to %d) or 0 to cancel: ", addressbook->count);

    int attempts = 0; // Counter for invalid attempts
    do
    {
        if (scanf("%d", &choice_select) != 1 || choice_select < 0 || choice_select > addressbook->count)
        {
            attempts++; // Increment the counter on invalid input
            printf("Invalid Input. You have %d attempt(s) left.\n", 3 - attempts);
            while (getchar() != '\n')
                ; // Clear the input buffer
            if (attempts >= 3)
            {
                printf("Too many invalid attempts. Exiting input.\n");
                return e_invalid; // Exit after 3 invalid attempts
            }
            continue;
        }

        if (choice_select == 0)
        {
            printf("Edit operation canceled.\n");
            return e_failure; // Exit if the user chooses to cancel
        }

        // If the input is valid, proceed
        printf("You selected contact #%d.\n", choice_select);
        break; // Exit the loop on valid input
    } while (1);

    ContactInfo *selectedContact = &addressbook->list[choice_select - 1];
    int editOption;

    // Start editing loop
    do
    {
        printf("\n####### Address Book #######\n");
        printf("Edit Contact:\n");
        printf("0. Back\n");
        printf("1. Name: %s\n", selectedContact->name);

        // Display phone numbers
        printf("2. Phone No \n");
        for (int i = 0; i < MAX_PHONE_NUMBERS && selectedContact->phone_number[i][0] != '\0'; i++)
        {
            // printf("2.%d. Phone No %d: %s\n", i + 1, i + 1, selectedContact->phone_number[i]);
            printf("            %d: %s\n", i + 1, selectedContact->phone_number[i]);
        }
        // Display email addresses
        printf("3. Email ID \n");
        for (int i = 0; i < MAX_EMAIL_IDS && selectedContact->email_addresses[i][0] != '\0'; i++)
        {
            //    printf("3.%d. Email ID %d: %s\n", i + 1, i + 1, selectedContact->email_addresses[i]);
            printf("            %d: %s\n", i + 1, selectedContact->email_addresses[i]);
        }
        // Get option from user
        printf("Please select an option: ");
        if (scanf("%d", &editOption) != 1)
        {
            printf("Invalid input. Try again.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }

        while (getchar() != '\n')
            ; // Clear input buffer after valid input

        if (editOption == 0)
        {
            printf("Exiting contact edit menu.\n");
            break;
        }

        if (editOption == 1)
        { // Edit Name
            printf("Enter new Name: ");
            fgets(selectedContact->name, sizeof(selectedContact->name), stdin);
            selectedContact->name[strcspn(selectedContact->name, "\n")] = '\0';
        }
        else if (editOption == 2)
        { // Phone number handling
            printf("Select Phone Number to edit (1 to %d) or 0 to add new phone number: ", MAX_PHONE_NUMBERS);
            int phoneChoice;
            if (scanf("%d", &phoneChoice) != 1 || phoneChoice < 0 || phoneChoice > MAX_PHONE_NUMBERS)
            {
                printf("Invalid input. Try again.\n");
                while (getchar() != '\n')
                    ; // Clear input buffer
                continue;
            }

            // Clear the input buffer to handle the next input correctly
            while (getchar() != '\n')
                ;

            if (phoneChoice == 0)
            { // Add new phone number
                for (int i = 0; i < MAX_PHONE_NUMBERS; i++)
                {
                    if (selectedContact->phone_number[i][0] == '\0')
                    { // Find first empty slot
                        printf("Enter new Phone Number: ");
                        fgets(selectedContact->phone_number[i], sizeof(selectedContact->phone_number[i]), stdin);
                        selectedContact->phone_number[i][strcspn(selectedContact->phone_number[i], "\n")] = '\0'; // Remove newline
                        break;
                    }
                }
            }
            else if (phoneChoice >= 1 && phoneChoice <= MAX_PHONE_NUMBERS)
            { // Edit existing phone number
                printf("Enter Phone Number %d: [Press Enter to remove]: ", phoneChoice);
                fgets(selectedContact->phone_number[phoneChoice - 1], sizeof(selectedContact->phone_number[phoneChoice - 1]), stdin);
                selectedContact->phone_number[phoneChoice - 1][strcspn(selectedContact->phone_number[phoneChoice - 1], "\n")] = '\0'; // Remove newline
            }
            else
            {
                printf("Invalid phone number choice.\n");
            }
        }
        else if (editOption == 3)
        { // Email ID handling
            printf("Select Email ID to edit (1 to %d) or 0 to add new email ID: ", MAX_EMAIL_IDS);
            int emailChoice;
            if (scanf("%d", &emailChoice) != 1 || emailChoice < 0 || emailChoice > MAX_EMAIL_IDS)
            {
                printf("Invalid input. Try again.\n");
                while (getchar() != '\n')
                    ; // Clear input buffer
                continue;
            }

            // Clear the input buffer to handle the next input correctly
            while (getchar() != '\n')
                ;

            if (emailChoice == 0)
            { // Add new email ID
                for (int i = 0; i < MAX_EMAIL_IDS; i++)
                {
                    if (selectedContact->email_addresses[i][0] == '\0')
                    { // Find first empty slot
                        printf("Enter new Email ID: ");
                        fgets(selectedContact->email_addresses[i], sizeof(selectedContact->email_addresses[i]), stdin);
                        selectedContact->email_addresses[i][strcspn(selectedContact->email_addresses[i], "\n")] = '\0'; // Remove newline
                        break;
                    }
                }
            }
            else if (emailChoice >= 1 && emailChoice <= MAX_EMAIL_IDS)
            { // Edit existing email ID
                printf("Enter Email ID %d: [Press Enter to remove]: ", emailChoice);
                fgets(selectedContact->email_addresses[emailChoice - 1], sizeof(selectedContact->email_addresses[emailChoice - 1]), stdin);
                selectedContact->email_addresses[emailChoice - 1][strcspn(selectedContact->email_addresses[emailChoice - 1], "\n")] = '\0'; // Remove newline
            }
            else
            {
                printf("Invalid email ID choice.\n");
            }
        }
        else
        {
            printf("Invalid option. Try again.\n");
        }
    } while (1);

    return e_success;
}

Status Delete_Contact(AddressBookInfo *addressbook)
{
    if (addressbook->count == 0)
    {
        printf("No contacts available to delete.\n");
        return e_invalid;
    }

    // Display all contacts
    printf("\n####### Address Book #######\n");
    //   diplay_contact_addressbook(addressbook);

    for (int i = 0; i < addressbook->count; i++)
    {
        printf("%d. %s\n", addressbook->list[i].Serial_No, addressbook->list[i].name);
    }

    // Get user input for the contact to delete
    int serial_number;
    printf("Enter the Serial No of the contact to delete (1 to %d) or 0 to cancel: ", addressbook->count);
    if (scanf("%d", &serial_number) != 1 || serial_number < 0 || serial_number > addressbook->count)
    {
        printf("Invalid input. Operation canceled.\n");
        return e_invalid;
    }

    if (serial_number == 0)
    {
        printf("Delete operation canceled.\n");
        return e_success;
    }

    // Confirm deletion
    printf("Are you sure you want to delete contact #%d? (y/n): ", serial_number);
    char confirmation;
    scanf(" %c", &confirmation);
    if (confirmation != 'y' && confirmation != 'Y')
    {
        printf("Delete operation canceled.\n");
        return e_success;
    }

    // Perform deletion by shifting contacts
    for (int i = serial_number - 1; i < addressbook->count - 1; i++)
    {
        addressbook->list[i] = addressbook->list[i + 1];
    }

    // Decrease the contact count
    addressbook->count--;

    printf("Contact #%d has been successfully deleted.\n", serial_number);
    return e_success;
}

Status diplay_contact_addressbook(AddressBookInfo *addressbook)
{
    // Check if address book is empty
    if (addressbook->count == 0)
    {
        printf("Address Book is Empty.\n");
        return e_invalid;
    }
    printf("#### Contact List ####\n");
    printf("============================================================\n");
    printf("S.No :    Name        :  Phone No         : Email ID\n");
    printf("============================================================\n");

    // Iterate through all contacts
    for (int i = 0; i < addressbook->count; i++)
    {
        // Print the first row of contact details
        printf("%d    : %s       : %s        : %s   \n",
               addressbook->list[i].Serial_No,
               addressbook->list[i].name,
               addressbook->list[i].phone_number[0],
               addressbook->list[i].email_addresses[0]);

        // Print additional phone numbers and email addresses, if any
        int j;
        for (j = 1; j < MAX_PHONE_NUMBERS; j++)
        {
            // Check if there's a phone number or email address to print
            if (strlen(addressbook->list[i].phone_number[j]) > 0 ||
                strlen(addressbook->list[i].email_addresses[j]) > 0)
            {
                printf("                      : %s        : %s \n",
                       addressbook->list[i].phone_number[j],
                       addressbook->list[i].email_addresses[j]);
            }
        }
        printf("\n\n");

        // Print empty rows if no additional contacts
        while (j < 5) // Assuming you want 5 total rows per contact
        {
            printf(" : : : : :\n");
            j++;
        }
    }
    printf("============================================================\n");
}

Status List_Contact(AddressBookInfo *addressbook)
{
    // Check if address book is empty
    if (addressbook->count == 0)
    {
        printf("Address Book is Empty.\n");
        return e_invalid;
    }

    char quit;
    do
    {
        diplay_contact_addressbook(addressbook);

        printf("Press [q] to Cancel : ");

        // Read quit character
        if (scanf(" %c", &quit) != 1)
        {
            printf("Invalid input. Defaulting to quit.\n");
            quit = 'q';
        }

        // Clear input buffer
        while (getchar() != '\n')
            ;

    } while (quit != 'q' && quit != 'Q');

    return e_success;
}
Status exit_menu(AddressBookInfo *addressbook)
{
    char option;
    // int isSave;
    printf("Enter 'N' to Ignore and 'Y' to Save: ");
    if (scanf(" %c", &option) != 1)
    {
        printf("Invalid Input\n");
        return e_invalid;
    }

    // Save the file if user chooses to save
    if (option == 'Y' || option == 'y')
    {
        if (isSave == 1 || dummysave == 1)
        {
            printf("Already File is File \n");
        }
        else
        {
            if (Save_File(addressbook) == e_success)
            {
                // Ensure the file pointer is not NULL before attempting to close
                if (addressbook->fp != NULL)
                {
                    fclose(addressbook->fp); // Properly close the file
                    addressbook->fp = NULL;  // Set the pointer to NULL after closing
                    printf("File Closed Successfully\n");
                }
                else
                {
                    // Attempt to reopen the file if the pointer is NULL
                    addressbook->fp = fopen(addressbook->default_name, "w");
                    if (addressbook->fp == NULL)
                    {
                        printf("Error: Failed to open the file for writing.\n");
                        return e_failure;
                    }
                    else
                    {
                        fclose(addressbook->fp); // Close it again if opened successfully
                        addressbook->fp = NULL;
                        printf("File Closed Successfully after reopening.\n");
                    }
                }
            }
            else
            {
                printf("Error: Save_File failed.\n");
                return e_failure;
            }
            return e_success;
        }
    }

    // If user chooses to ignore saving
    else if (option == 'n' || option == 'N')
    {
        if (addressbook->fp != NULL)
        {
            if (fclose(addressbook->fp) != 0)
            {
                printf("Error: Failed to close the file properly. Check if the file is still in use or locked.\n");
                return e_failure;
            }
            printf("File closed successfully without saving.\n");
            addressbook->fp = NULL; // Reset the pointer to avoid dangling references
        }
        else
        {
            printf("No file was open. Exiting without saving changes.\n");
        }
        return e_success; // Exiting without saving is not an error
    }

    else
    {
        printf("Invalid option selected.\n");
        return e_invalid;
    }
    //   }
}
