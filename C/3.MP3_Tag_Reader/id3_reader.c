/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_reader.h"
#include "error_handling.h"
#include "main.h"

#define TAG_SIZE 128

const char *genres[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
    "Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae",
    "Rock", "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
    "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion",
    "Trance", "Classical", "Instrumental", "Acid", "House", "Game", "Sound Clip",
    "Gospel", "Noise", "AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative",
    "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
    "Electronic", "Pop-Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult",
    "Gangsta", "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
    "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi", "Tribal",
    "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock"
};

TagData *read_id3_tags(const char *filename)
{
    // Implementation for reading ID3 tags
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Failed to open file\n");
        return NULL;
    }

    if (fseek(fp, -TAG_SIZE, SEEK_END) != 0)
    {
        printf("Error seeking file\n");
        fclose(fp);
        return NULL;
    }

    char tag[TAG_SIZE];
    size_t bytesRead = fread(tag, sizeof(char), TAG_SIZE, fp);

    if (bytesRead < TAG_SIZE || (strncmp(tag, "TAG", 3) != 0))
    {
        fclose(fp);
        return NULL;
    }

    TagData *data = malloc(sizeof(TagData));
    if (!data)
    {
        fclose(fp);
        printf("Failed to allocate memory for Tagdata\n");
        return NULL;
    }
    data->version = strdup("ID3v1");
    data->title = strndup(tag + 3, 30);
    data->artist = strndup(tag + 33, 30);
    data->album = strndup(tag + 63, 30);
    data->year = strndup(tag + 93, 4);
    data->comment = strndup(tag + 97, 30);
    data->genre = malloc(sizeof(char));

    // Get genre
    unsigned char genre_code = tag[127];
    if (genre_code < sizeof(genres) / sizeof(genres[0]))
    {
        data->genre = strdup(genres[genre_code]);
    }
    else
    {
        data->genre = strdup("Unknown");
    }

    return data;
}

/// @brief
/// @param data
/*
TagData *read_id32_tags(const char *filename)
{
    // Open the file in binary read mode
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Failed to open file\n");
        return NULL;
    }

    char header[10];
    size_t bytesRead = fread(header, sizeof(char), 10, fp);

    if(bytesRead < 10 || strncmp(header, "ID3", 3) != 0)
    {
        fclose(fp);  // Close the file if ID3 header is not found
        return NULL;
    }

    unsigned char version[2];
    unsigned int size;
    memcpy(version, header + 3, 2);
    size = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | (header[9]);

    printf("ID3v2.%d.%d detected\n", version[0], version[1]);


    // Read the first frame header (optional, depending on how much tag data you want to extract)
    fseek(fp, 10, SEEK_SET);
    char frame_header[10];
    fread(frame_header, sizeof(char), 10, fp);

    // Allocate memory for the TagData structure
    TagData *data = malloc(sizeof(TagData));
    if (!data)
    {
        fclose(fp);  // Close the file if memory allocation fails
        printf("Failed to allocate memory for TagData\n");
        return NULL;
    }

    // Store version information in the TagData structure
    data->version = strdup("ID3v2");

    // Close the file and return the data
    fclose(fp);
    return data;
}
*/

TagData *read_id32_tags(const char *filename)
{
    // Open the file in binary read mode
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Failed to open file\n");
        return NULL;
    }

    char header[10];
    size_t bytesRead = fread(header, sizeof(char), 10, fp);

    if (bytesRead < 10 || strncmp(header, "ID3", 3) != 0)
    {
        fclose(fp); // Close the file if ID3 header is not found
        return NULL;
    }

    unsigned char version[2];
    unsigned int size;
    memcpy(version, header + 3, 2);
    size = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | (header[9]);

    // Allocate memory for the TagData structure
    TagData *data = malloc(sizeof(TagData));
    if (!data)
    {
        fclose(fp); // Close the file if memory allocation fails
        printf("Failed to allocate memory for TagData\n");
        return NULL;
    }

    // Store the full version in the TagData structure
    char version_str[10];
    snprintf(version_str, sizeof(version_str), "ID3v2.%d.%d", version[0], version[1]);
    data->version = strdup(version_str);

    // Close the file and return the data
    fclose(fp);
    return data;
}

void display_metadata(const TagData *data)
{
    // Check if the data pointer is NULL
    if (data == NULL)
    {
        printf("Error: No metadata available.\n");
        return;
    }
    // Display each piece of metadata, checking for NULL strings
    printf("Metadata:\n");
    printf("Version : %s\n", data->version ? data->version : "N/A");
    printf("Title   : %s\n", data->title ? data->title : "N/A");
    printf("Artist  : %s\n", data->artist ? data->artist : "N/A");
    printf("Album   : %s\n", data->album ? data->album : "N/A");
    printf("Year    : %s\n", data->year ? data->year : "N/A");
    printf("Comment : %s\n", data->comment ? data->comment : "N/A");
    printf("Genre   : %s\n", data->genre ? data->genre : "N/A");
}

void view_tags(const char *filename)
{
    TagData *data = read_id3_tags(filename);
    if (!data)
    {
        display_error("Failed to read ID3 tags.");
        return;
    }
    //   printf("%s\n", filename);

    TagData *data1 = read_id32_tags(filename);
    if (data1)
    {
        free(data->version);
        data->version = data1->version;
        // free_tag_data(data1);
    }

    display_metadata(data);
    free_tag_data(data);
}
