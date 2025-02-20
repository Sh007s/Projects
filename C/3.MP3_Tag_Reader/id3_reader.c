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
#include <ctype.h>

// Define MIN macro at the top
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TAG_SIZE 128
#define ID3V2_HEADER_SIZE 10

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
    "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock"};

// Predefined ID3v1 genres array
const char *id3v1_genres[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
    "Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
    "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
    "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance", "Classical",
    "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise", "AlternRock",
    "Bass", "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
    "Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
    "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap",
    "Pop/Funk", "Jungle", "Native American", "Cabaret", "New Wave", "Psychedelic", "Rave",
    "Showtunes", "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro",
    "Musical", "Rock & Roll", "Hard Rock", "Unknown"};

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
    printf("%ld\n", bytesRead);

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
    //  data->genre = malloc(sizeof(char));

    // Get genre
    unsigned char genre_code = tag[127];
    printf("Genre code: %d\n", genre_code); // Debugging
    if (genre_code == 255)
    {
        //      data->genre = strdup("No Genre");
    }
    else if (genre_code < sizeof(genres) / sizeof(genres[0]))
    {
        data->genre = strdup(genres[genre_code]);
    }
    else
    {
        //    data->genre = strdup("Unknown");
    }

    return data;
}

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
    printf("%ld\n", bytesRead);

    if (bytesRead < 10 || strncmp(header, "ID3", 3) != 0)
    {
        fclose(fp); // Close the file if ID3 header is not found
        return NULL;
    }

    unsigned char version[2] = {header[3], header[4]};
    unsigned int size;
    // memcpy(version, header + 3, 2);
    size = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | (header[9]);

    //  size = ((header[6] & 0x7F) << 21) | ((header[7] & 0x7F ) << 14) | ((header[8] & 0x7F)  << 7) | ((header[9] & 0x7F) );

    char *buff = malloc(size + 1);
    if (!buff)
    {
        fclose(fp);
        return NULL;
    }
    fread(buff, 1, size, fp);
    buff[size] = '\0';
    fclose(fp);

    printf("Size of %d\n", size);
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

    for (int i = 0; i < size - 10; i++)
    {
        if (strncmp(&buff[i], "TCON", 4) == 0)
        {
            int frame_size = (buff[i + 4] << 24) | (buff[i + 5] << 16) |
                             (buff[i + 6] << 8) | (buff[i + 7]);
            if (frame_size > 0 && i + 11 + frame_size <= size)
            {
                char *genre = strndup(&buff[i + 11], frame_size - 1);
            }
            break;

            // free(buff);
            // return genre;
        }
    }
    // Close the file and return the data
    fclose(fp);
    return data;
}
*/

TagData *read_id32_tags(const char *filename)
{
    // Open file
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Failed to open file\n");
        return NULL;
    }

    // Read ID3v2 header
    char header[ID3V2_HEADER_SIZE];
    if (fread(header, 1, ID3V2_HEADER_SIZE, fp) < ID3V2_HEADER_SIZE || strncmp(header, "ID3", 3) != 0)
    {
        fclose(fp);
        return NULL; // Not an ID3v2 file
    }

    // Extract version and tag size
    unsigned char version[2] = {header[3], header[4]};
    unsigned int size = ((header[6] & 0x7F) << 21) |
                        ((header[7] & 0x7F) << 14) |
                        ((header[8] & 0x7F) << 7) |
                        (header[9] & 0x7F);

    // Allocate buffer for ID3 data
    char *buff = malloc(size + 1);
    if (!buff)
    {
        fclose(fp);
        return NULL;
    }

    fread(buff, 1, size, fp);
    buff[size] = '\0'; // Null-terminate the buffer
    fclose(fp);

    // Allocate memory for TagData
    TagData *data = malloc(sizeof(TagData));
    if (!data)
    {
        free(buff);
        return NULL;
    }
    memset(data, 0 ,sizeof(TagData));
    // Store version
    char version_str[10];
    snprintf(version_str, sizeof(version_str), "ID3v2.%d.%d", version[0], version[1]);
    data->version = strdup(version_str);
    data->genre = strdup(""); // Default genre
    printf("Starting tag parsing...\n");
    int found_tcon = 0;

    // Search for TCON (Genre) frame
    for (int i = 0; i < size - 10; i++)
    {
        if (strncmp(&buff[i], "TCON", 4) == 0)
        {
            found_tcon = 1;
            printf("Found TCON frame at offset %d\n", i);

            int frame_size = ((buff[i + 4] & 0x7F) << 21) |
                             ((buff[i + 5] & 0x7F) << 14) |
                             ((buff[i + 6] & 0x7F) << 7) |
                             (buff[i + 7] & 0x7F);

            printf("Frame size: %d bytes\n", frame_size);

            if (frame_size > 0 && i + 11 + frame_size <= size)
            {
                char *genre_start = &buff[i + 11];
                unsigned char encoding = (unsigned char)genre_start[0];

                printf("Raw frame data (hex): ");
                for (int j = 0; j < frame_size; j++)
                {
                    printf("%02x ", (unsigned char)genre_start[j]);
                }
                printf("\n");

                // Free existing genre
                if (data->genre)
                {
                    free(data->genre);
                    data->genre = NULL;
                }

                // Check for UTF-16 encoding
                if (encoding == 1 && frame_size > 3) // UTF-16 with encoding byte
                {
                    // Skip encoding byte
                    genre_start++;
                    frame_size--;

                    // Skip BOM if present
                    if (frame_size >= 2 &&
                        ((unsigned char)genre_start[0] == 0xFF && (unsigned char)genre_start[1] == 0xFE))
                    {
                        genre_start += 2;
                        frame_size -= 2;
                    }

                    // Convert UTF-16 to ASCII
                    char *ascii_genre = malloc(frame_size / 2 + 1);
                    int ascii_pos = 0;

                    for (int j = 0; j < frame_size - 1; j += 2)
                    {
                        if (genre_start[j] && !genre_start[j + 1] && isprint(genre_start[j]))
                        {
                            ascii_genre[ascii_pos++] = genre_start[j];
                        }
                    }
                    ascii_genre[ascii_pos] = '\0';
                    data->genre = ascii_genre;
                }
                else // ASCII or ISO-8859-1
                {
                    // Skip encoding byte if it's 0
                    if (encoding == 0)
                    {
                        genre_start++;
                        frame_size--;
                    }
                    data->genre = strndup(genre_start, frame_size);
                }

                // Clean up the genre string
                if (data->genre)
                {
                    int len = strlen(data->genre);
                    // Remove trailing 'C' if present
                    if (len > 0 && data->genre[len - 1] == 'C')
                    {
                        data->genre[len - 1] = '\0';
                        len--;
                    }

                    // Remove any trailing whitespace or control characters
                    while (len > 0 && (data->genre[len - 1] == ' ' ||
                                       data->genre[len - 1] == 0 ||
                                       data->genre[len - 1] == '\r' ||
                                       data->genre[len - 1] == '\n'))
                    {
                        data->genre[len - 1] = '\0';
                        len--;
                    }

                    // If empty after cleanup, set to Unknown
                    if (len == 0)
                    {
                        free(data->genre);
                        data->genre = strdup("Unknown Genre");
                    }

                    printf("Final genre: '%s'\n", data->genre);
                }
            }
            else
            {
                printf("Invalid frame size or bounds\n");
            }
            break;
        }
    }

    if (!found_tcon)
    {
        printf("No TCON frame found in file\n");
        if (data->genre)
        {
            free(data->genre);
        }
        data->genre = strdup("No Genre");
    }

    free(buff);
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

    display_metadata(data);
    free_tag_data(data);  // Free allocated memory after usage

//     TagData *data1 = read_id32_tags(filename);
//     if (!data1)
//     {
//         display_error("Failed to read ID3v2 tags.");
//         return;
//     }

//     display_metadata(data1);
//    // free_tag_data(data1);  // Free allocated memory after usage
}
