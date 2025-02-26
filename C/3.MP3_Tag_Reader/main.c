#include "main.h"
#include "error_handling.h"
#include "id3_reader.h"
#include "id3_writer.h"
#include <stdio.h>
#include <string.h>

void display_help()
{
    printf("Help menu for MP3 Tag Reader and Editor: \n\n");
    printf("For viewing the tags-          ./mp3_tag_reader -v <filename.mp3>\n");
    printf("\nFor editing the tags -         ./mp3_tag_reader -e <filename.mp3>  "
           "<modifier>\n");
    printf("\nModifier Function\n\n");
    printf("    -t      Modity Title Tag\n");
    printf("    -T      Modity Track Tag\n");
    printf("    -a      Modity Artist Tag\n");
    printf("    -A      Modity Album Tag\n");
    printf("    -y      Modity Year Tag\n");
    printf("    -c      Modity Comment Tag\n");
    printf("    -g      Modity Genre Tag\n");
}

/**
 * @brief Main function to handle command-line arguments and execute appropriate
 * actions.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, non-zero on failure.
 */
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("ERROR: Incorrect format of Command line Arguments.\n");
        printf("Use ./mp3_tag_reader -h for help\n");
        return e_failed;
    }
    if (argc < 2)
    {
        printf("Usage : %s <Filename>\n", argv[0]); //
        return e_failed;
    }

    if (strcmp(argv[1], "-h") == 0)
    {
        display_help();
        return e_success;
    }
    else if (strcmp(argv[1], "-v") == 0 && argc == 3)
    {
        if (strstr(argv[2], ".mp3") == 0)
        {
            printf("Error: argv[3] is not passed properly.\n");
            printf("Usage: ./a.out -v filename.mp3 \n");
            return e_success;
        }
        char *ext = strrchr(argv[2], '.');

        if (ext != NULL && strcmp(ext, ".mp3") == 0)
        {
             view_tags(argv[2]);
        }
        else
        {
            printf("Failed\n");
        }
    }

    else if (strcmp(argv[1], "-e") == 0 && argc == 5)
    {
        char *tag = argv[2];
        char *filename = argv[3];
        char *value = argv[4];
        if (edit_tag(filename, tag, value) != 0)
        {
            display_error("Failed to edit tag.");
            return e_failed;
        }
        printf("Tag edited successfully.\n");
    }
    else
    {
        printf("ERROR: Incorrect format of Command line Arguments.\n");
        printf("Use ./mp3_tag_reader -h for help\n\t\t\t\tor\n");
        display_help();

        return e_failed;
    }

    return e_success;
}
