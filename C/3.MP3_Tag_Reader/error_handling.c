#include <stdio.h>
#include "error_handling.h"

void display_error(const char *message)
{
    if (message != NULL)
    {
        printf("Error : %s\n", message);
    }
    else
    {
        printf("Error: An unknown error occurred.\n");
    }
}