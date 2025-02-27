/**
 * @file id3_writer.c
 * @brief Implementation of functions for writing and editing ID3 tags in MP3 files.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "main.h"
#include "id3_writer.h"
#include "id3_reader.h"
#include "error_handling.h"
#include "id3_utils.h"
#include "genre_utils.h"

/**
 * @brief Determine which ID3 version to use based on the filename
 *
 * @param filename The name of the MP3 file
 * @return int 1 for ID3v1, 2 for ID3v2, 0 on error
 */
int determine_id3_version(const char *filename)
{
    if (!filename)
        return 0;

    size_t len = strlen(filename);

    // Look for v1 or v2 indication in the filename
    if (len > 4)
    {
        // Check for ID3v2 indicators
        if (strstr(filename, "v2") || strstr(filename, "V2") ||
            strstr(filename, "id3v2") || strstr(filename, "ID3V2"))
        {
            return 2;
        }

        // Check for ID3v1 indicators
        if (strstr(filename, "v1") || strstr(filename, "V1") ||
            strstr(filename, "id3v1") || strstr(filename, "ID3V1"))
        {
            return 1;
        }
    }

    // Default to ID3v2 if no indication in filename
    return 2;
}

/**
 * @brief Encodes a regular integer to a synchsafe integer used in ID3 frames
 *
 * @param value The integer value to encode
 * @param bytes Output array to store the 4-byte synchsafe integer
 */
void encode_synchsafe(unsigned int value, unsigned char bytes[4])
{
    bytes[0] = (value >> 21) & 0x7F;
    bytes[1] = (value >> 14) & 0x7F;
    bytes[2] = (value >> 7) & 0x7F;
    bytes[3] = value & 0x7F;
}

/**
 * @brief Creates a new ID3v2 frame
 *
 * @param id The 4-character frame ID (e.g., "TIT2")
 * @param data The frame data (including encoding byte)
 * @param data_size The size of the frame data
 * @param output Buffer to store the complete frame
 * @param id3_version ID3v2 version (2 or 3)
 * @return The total size of the frame
 */
int create_id3v2_frame(const char *id, const unsigned char *data, int data_size,
                       unsigned char *output, int id3_version)
{
    int index = 0;
    int frame_id_size = (id3_version == 2) ? 3 : 4;

    // Copy frame ID
    memcpy(output + index, id, frame_id_size);
    index += frame_id_size;

    // Set frame size
    if (id3_version == 2)
    {
        // ID3v2.2 uses 3 bytes for size
        output[index++] = (data_size >> 16) & 0xFF;
        output[index++] = (data_size >> 8) & 0xFF;
        output[index++] = data_size & 0xFF;
    }
    else
    {
        // ID3v2.3 uses 4 bytes for size
        output[index++] = (data_size >> 24) & 0xFF;
        output[index++] = (data_size >> 16) & 0xFF;
        output[index++] = (data_size >> 8) & 0xFF;
        output[index++] = data_size & 0xFF;

        // Add flags (two bytes, all zeros)
        output[index++] = 0;
        output[index++] = 0;
    }

    // Copy frame data
    memcpy(output + index, data, data_size);
    index += data_size;

    return index;
}

/**
 * @brief Creates a text frame for ID3v2 tag
 *
 * @param id The 4-character frame ID (e.g., "TIT2")
 * @param text The text to include in the frame
 * @param output Buffer to store the complete frame
 * @param id3_version ID3v2 version (2 or 3)
 * @return The total size of the frame
 */
int create_text_frame(const char *id, const char *text, unsigned char *output,
                      int id3_version)
{
    if (!text || !output || !id)
        return 0;

    int text_len = strlen(text);
    if (text_len == 0)
        return 0;

    // Create frame data with encoding byte (0 = ISO-8859-1)
    unsigned char *frame_data = malloc(text_len + 1);
    if (!frame_data)
        return 0;

    frame_data[0] = 0; // ISO-8859-1 encoding
    memcpy(frame_data + 1, text, text_len);

    // Create the full frame
    int frame_size = create_id3v2_frame(id, frame_data, text_len + 1, output, id3_version);

    free(frame_data);
    return frame_size;
}

/**
 * @brief Writes ID3v2 tags to an MP3 file
 *
 * @param filename The name of the MP3 file
 * @param data Pointer to the TagData structure containing the tag data
 * @param id3_version ID3v2 version to use (2 or 3)
 * @return 0 on success, non-zero on failure
 */
int write_id3v2_tags(const char *filename, const TagData *data, int id3_version)
{
    if (!filename || !data || (id3_version != 2 && id3_version != 3))
    {
        display_error("Invalid parameters for write_id3v2_tags");
        return 1;
    }

    // Read the entire file into memory
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        display_error("Cannot open file for reading");
        return 1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Check for existing ID3v2 tag
    char header[10];
    if (fread(header, 1, 10, file) != 10)
    {
        fclose(file);
        display_error("Failed to read file header");
        return 1;
    }

    long audio_start = 0;
    if (strncmp(header, "ID3", 3) == 0)
    {
        // Calculate existing tag size
        unsigned int tag_size = decode_synchsafe((unsigned char *)header + 6);
        audio_start = tag_size + 10;
    }

    fseek(file, audio_start, SEEK_SET);

    // Read audio data
    unsigned char *audio_data = malloc(file_size - audio_start);
    if (!audio_data)
    {
        fclose(file);
        display_error("Memory allocation failed");
        return 1;
    }

    if (fread(audio_data, 1, file_size - audio_start, file) != file_size - audio_start)
    {
        free(audio_data);
        fclose(file);
        display_error("Failed to read audio data");
        return 1;
    }

    fclose(file);

    // Create new ID3v2 tag
    unsigned char *tag_buffer = calloc(1, MAX_TAG_SIZE);
    if (!tag_buffer)
    {
        free(audio_data);
        display_error("Memory allocation failed");
        return 1;
    }

    int tag_index = 10; // Leave space for header

    // Add frames if they exist
    if (data->title)
    {
        tag_index += create_text_frame(id3_version == 2 ? "TT2" : "TIT2",
                                       data->title, tag_buffer + tag_index, id3_version);
    }

    if (data->artist)
    {
        tag_index += create_text_frame(id3_version == 2 ? "TP1" : "TPE1",
                                       data->artist, tag_buffer + tag_index, id3_version);
    }

    if (data->album)
    {
        tag_index += create_text_frame(id3_version == 2 ? "TAL" : "TALB",
                                       data->album, tag_buffer + tag_index, id3_version);
    }

    if (data->year)
    {
        tag_index += create_text_frame(id3_version == 2 ? "TYE" : "TYER",
                                       data->year, tag_buffer + tag_index, id3_version);
    }

    if (data->genre)
    {
        tag_index += create_text_frame(id3_version == 2 ? "TCO" : "TCON",
                                       data->genre, tag_buffer + tag_index, id3_version);
    }

    if (data->comment)
    {
        // Comment frames are more complex, requiring language code
        unsigned char *comm_data = calloc(1, strlen(data->comment) + 5);
        if (comm_data)
        {
            comm_data[0] = 0;                // ISO-8859-1 encoding
            memcpy(comm_data + 1, "eng", 3); // Language code (English)
            comm_data[4] = 0;                // Empty description
            memcpy(comm_data + 5, data->comment, strlen(data->comment));

            tag_index += create_id3v2_frame(id3_version == 2 ? "COM" : "COMM",
                                            comm_data, strlen(data->comment) + 5,
                                            tag_buffer + tag_index, id3_version);
            free(comm_data);
        }
    }

    // Handle track number
    if (data->track > 0)
    {
        char track_str[16];
        if (data->total_track > 0)
        {
            snprintf(track_str, sizeof(track_str), "%d/%d", data->track, data->total_track);
        }
        else
        {
            snprintf(track_str, sizeof(track_str), "%d", data->track);
        }
        tag_index += create_text_frame(id3_version == 2 ? "TRK" : "TRCK",
                                       track_str, tag_buffer + tag_index, id3_version);
    }

    // Create the ID3v2 header
    memcpy(tag_buffer, "ID3", 3);
    tag_buffer[3] = id3_version;
    tag_buffer[4] = 0; // Revision
    tag_buffer[5] = 0; // Flags

    // Tag size (excluding header)
    unsigned int tag_size = tag_index - 10;
    encode_synchsafe(tag_size, tag_buffer + 6);

    // Write the new file
    file = fopen(filename, "wb");
    if (!file)
    {
        free(audio_data);
        free(tag_buffer);
        display_error("Cannot open file for writing");
        return 1;
    }

    // Write ID3v2 tag
    if (fwrite(tag_buffer, 1, tag_index, file) != tag_index)
    {
        free(audio_data);
        free(tag_buffer);
        fclose(file);
        display_error("Failed to write ID3v2 tag");
        return 1;
    }

    // Write audio data
    if (fwrite(audio_data, 1, file_size - audio_start, file) != file_size - audio_start)
    {
        free(audio_data);
        free(tag_buffer);
        fclose(file);
        display_error("Failed to write audio data");
        return 1;
    }

    free(audio_data);
    free(tag_buffer);
    fclose(file);

    printf("ID3v2.%d tags updated successfully.\n", id3_version);
    return 0;
}

/**
 * @brief Writes ID3v1 tags to an MP3 file
 *
 * @param filename The name of the MP3 file
 * @param data Pointer to the TagData structure containing the tag data
 * @return 0 on success, non-zero on failure
 */
int write_id3v1_tags(const char *filename, TagData *data)
{
    if (!filename || !data)
    {
        display_error("Invalid parameters for write_id3v1_tags");
        return 1;
    }

    FILE *file = fopen(filename, "r+b");
    if (!file)
    {
        display_error("Error opening file");
        printf("Error details: %s\n", strerror(errno));
        return 1;
    }

    // Check if we can write
    if (fseek(file, -128, SEEK_END) != 0)
    {
        display_error("Error seeking file");
        printf("Error details: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    // Set the TAG identifier
    strcpy(data->tag, "TAG");

    // Force a test write
    if (fputc('T', file) == EOF)
    {
        display_error("Test write failed");
        printf("Error details: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    // Go back to the ID3v1 tag position
    if (fseek(file, -128, SEEK_END) != 0)
    {
        display_error("Error seeking to ID3v1 position");
        fclose(file);
        return 1;
    }

    // Prepare fields for ID3v1
    char title_buffer[30] = {0};
    char artist_buffer[30] = {0};
    char album_buffer[30] = {0};
    char year_buffer[4] = {0};
    char comment_buffer[28] = {0};

    // Copy data with proper limits
    if (data->title)
        strncpy(title_buffer, data->title, 29);
    if (data->artist)
        strncpy(artist_buffer, data->artist, 29);
    if (data->album)
        strncpy(album_buffer, data->album, 29);
    if (data->year)
        strncpy(year_buffer, data->year, 4);
    if (data->comment)
        strncpy(comment_buffer, data->comment, 27);

    // Get genre code if genre is provided as string
    unsigned char genre_code = 12; // Default to "Other" genre
    if (data->genre_code != 0)
    {
        genre_code = data->genre_code;
    }
    else if (data->genre)
    {
        // Convert text genre to code (simplified example)
        int code = get_genre_code_by_name(data->genre);
        if (code >= 0)
            genre_code = (unsigned char)code;
    }

    // Write the ID3v1 tag
    if (fwrite(data->tag, 1, 3, file) != 3 ||
        fwrite(title_buffer, 1, 30, file) != 30 ||
        fwrite(artist_buffer, 1, 30, file) != 30 ||
        fwrite(album_buffer, 1, 30, file) != 30 ||
        fwrite(year_buffer, 1, 4, file) != 4 ||
        fwrite(comment_buffer, 1, 28, file) != 28 ||
        fputc(0, file) == EOF || // Zero byte for ID3v1.1
        fputc(data->track, file) == EOF ||
        fputc(genre_code, file) == EOF)
    {
        display_error("Error writing ID3 tags");
        printf("Error details: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    fclose(file);
    printf("ID3v1 tags updated successfully.\n");
    return 0;
}

/**
 * @brief Writes ID3 tags to an MP3 file based on the filename
 *
 * @param filename The name of the MP3 file
 * @param data Pointer to the TagData structure containing the tag data
 * @return 0 on success, non-zero on failure
 */
int write_id3_tags(const char *filename, TagData *data)
{
    if (!filename || !data)
    {
        display_error("Invalid parameters for write_id3_tags");
        return 1;
    }

    // Determine ID3 version based on filename
    int id3_version = determine_id3_version(filename);

    if (id3_version == 1)
    {
        // Use ID3v1 tags
        printf("Using ID3v1 tags based on filename.\n");
        return write_id3v1_tags(filename, data);
    }
    else if (id3_version == 2)
    {
        // Use ID3v2 tags
        printf("Using ID3v2 tags based on filename.\n");
        int subversion = 3; // Default to ID3v2.3
        if (data->version && strncmp(data->version, "2.", 2) == 0)
        {
            int ver = data->version[2] - '0';
            if (ver == 2 || ver == 3)
            {
                subversion = ver;
            }
        }
        return write_id3v2_tags(filename, data, subversion);
    }
    else
    {
        display_error("Failed to determine ID3 version from filename");
        return 1;
    }
}

/**
 * @brief Edits a specific ID3 tag in an MP3 file based on filename
 *
 * @param filename The name of the MP3 file to edit
 * @param tag The tag identifier to edit (-t for title, -a for artist, etc.)
 * @param value The new value to set for the specified tag
 * @return e_success on success, e_failed on failure
 */
int edit_tag(const char *filename, const char *tag, const char *value)
{
    if (!filename || !tag || !value)
    {
        display_error("Invalid parameters for edit_tag function");
        return e_failed;
    }

    // Determine which version of ID3 to use
    int id3_version = determine_id3_version(filename);

    if (id3_version == 1)
    {
        // Read existing ID3v1 tags
        TagData *data = read_id3v1_tags(filename);
        if (!data)
        {
            printf("No existing ID3v1 tags found. Creating new tags.\n");
            data = calloc(1, sizeof(TagData));
            if (!data)
            {
                display_error("Failed to allocate memory for new tags");
                return e_failed;
            }
            // Initialize default values
            data->track = 0;
            data->genre_code = 12; // Default to "Other" genre
        }

        // Update the appropriate tag field
        if (strcmp(tag, "-t") == 0 || strcmp(tag, "--title") == 0)
        {
            printf("Modifying the title\n");
            SAFE_FREE(data->title);
            data->title = strdup(value);
        }
        else if (strcmp(tag, "-a") == 0 || strcmp(tag, "--artist") == 0)
        {
            printf("Modifying the artist\n");
            SAFE_FREE(data->artist);
            data->artist = strdup(value);
        }
        else if (strcmp(tag, "-A") == 0 || strcmp(tag, "--album") == 0)
        {
            printf("Modifying the album\n");
            SAFE_FREE(data->album);
            data->album = strdup(value);
        }
        else if (strcmp(tag, "-y") == 0 || strcmp(tag, "--year") == 0)
        {
            printf("Modifying the year\n");
            SAFE_FREE(data->year);
            data->year = strdup(value);
        }
        else if (strcmp(tag, "-c") == 0 || strcmp(tag, "--comment") == 0)
        {
            printf("Modifying the comment\n");
            SAFE_FREE(data->comment);
            data->comment = strdup(value);
        }
        else if (strcmp(tag, "-T") == 0 || strcmp(tag, "--track") == 0)
        {
            printf("Modifying the track number\n");
            int track_num = atoi(value);
            if (track_num >= 0 && track_num <= 255)
            {
                data->track = (unsigned char)track_num;
            }
            else
            {
                printf("Warning: Track number out of range (0-255). Using 0.\n");
                data->track = 0;
            }
        }
        else if (strcmp(tag, "-g") == 0 || strcmp(tag, "--genre") == 0)
        {
            printf("Modifying the genre\n");
            // Process genre (either code or text)
            char *endptr;
            long genre_code = strtol(value, &endptr, 10);
            if (*endptr == '\0' && genre_code >= 0 && genre_code <= 255)
            {
                data->genre_code = (unsigned char)genre_code;
                SAFE_FREE(data->genre);
            }
            else
            {
                SAFE_FREE(data->genre);
                data->genre = strdup(value);
                data->genre_code = get_genre_code_by_name(value);
            }
        }
        else
        {
            display_error("Unknown tag specifier");
            printf("Valid options: -t/--title, -a/--artist, -A/--album, -y/--year\n");
            printf("               -c/--comment, -T/--track, -g/--genre\n");
            free_tag_data(data);
            return e_failed;
        }

        // Write updated tags back to file
        if (write_id3v1_tags(filename, data) != 0)
        {
            display_error("Failed to update ID3v1 tags");
            free_tag_data(data);
            return e_failed;
        }

        free_tag_data(data);
        return e_success;
    }
    else if (id3_version == 2)
    {
        // Read existing ID3v2 tags
        TagData *data = read_id3v2_tags(filename);
        if (!data)
        {
            printf("No existing ID3v2 tags found. Creating new tags.\n");
            data = calloc(1, sizeof(TagData));
            if (!data)
            {
                display_error("Failed to allocate memory for new tags");
                return e_failed;
            }
            data->version = strdup("2.3.0"); // Default to ID3v2.3
        }

        // Determine ID3v2 version
        int subversion = 3; // Default to 2.3
        if (data->version && strncmp(data->version, "2.", 2) == 0)
        {
            subversion = data->version[2] - '0';
            if (subversion != 2 && subversion != 3)
            {
                subversion = 3; // Default to 2.3 if version is not 2.2 or 2.3
            }
        }

        // Update the appropriate tag field
        if (strcmp(tag, "-t") == 0 || strcmp(tag, "--title") == 0)
        {
            printf("Modifying the title\n");
            SAFE_FREE(data->title);
            data->title = strdup(value);
        }
        else if (strcmp(tag, "-a") == 0 || strcmp(tag, "--artist") == 0)
        {
            printf("Modifying the artist\n");
            SAFE_FREE(data->artist);
            data->artist = strdup(value);
        }
        else if (strcmp(tag, "-A") == 0 || strcmp(tag, "--album") == 0)
        {
            printf("Modifying the album\n");
            SAFE_FREE(data->album);
            data->album = strdup(value);
        }
        else if (strcmp(tag, "-y") == 0 || strcmp(tag, "--year") == 0)
        {
            printf("Modifying the year\n");
            SAFE_FREE(data->year);
            data->year = strdup(value);
        }
        else if (strcmp(tag, "-c") == 0 || strcmp(tag, "--comment") == 0)
        {
            printf("Modifying the comment\n");
            SAFE_FREE(data->comment);
            data->comment = strdup(value);
        }
        else if (strcmp(tag, "-T") == 0 || strcmp(tag, "--track") == 0)
        {
            printf("Modifying the track number\n");
            int track_num = atoi(value);
            if (track_num >= 0)
            {
                data->track = track_num;
            }
            else
            {
                printf("Warning: Invalid track number. Using 0.\n");
                data->track = 0;
            }
        }
        else if (strcmp(tag, "-g") == 0 || strcmp(tag, "--genre") == 0)
        {
            printf("Modifying the genre\n");
            SAFE_FREE(data->genre);
            // Normalize the genre string
            char *normalized_genre = normalize_genre(value);
            data->genre = normalized_genre;
        }
        else
        {
            display_error("Unknown tag specifier");
            printf("Valid options: -t/--title, -a/--artist, -A/--album, -y/--year\n");
            printf("               -c/--comment, -T/--track, -g/--genre\n");
            free_tag_data(data);
            return e_failed;
        }

        // Write updated tags back to file
        if (write_id3v2_tags(filename, data, subversion) != 0)
        {
            display_error("Failed to update ID3v2 tags");
            free_tag_data(data);
            return e_failed;
        }

        free_tag_data(data);
        return e_success;
    }
    else
    {
        display_error("Failed to determine ID3 version from filename");
        return e_failed;
    }
}

/**
 * @brief Sets multiple ID3 tags in an MP3 file at once based on filename
 *
 * @param filename The name of the MP3 file to edit
 * @param new_data Pointer to the TagData structure containing the new tag values
 * @return e_success on success, e_failed on failure
 */
int set_all_tags(const char *filename, const TagData *new_data)
{
    if (!filename || !new_data)
    {
        display_error("Invalid parameters for set_all_tags function");
        return e_failed;
    }

    // Determine which version of ID3 to use
    int id3_version = determine_id3_version(filename);

    if (id3_version == 1)
    {
        // Handle ID3v1 tags
        TagData *data = calloc(1, sizeof(TagData));
        if (!data)
        {
            display_error("Failed to allocate memory for tags");
            return e_failed;
        }

        // Copy all the tag data
        if (new_data->title)
            data->title = strdup(new_data->title);
        if (new_data->artist)
            data->artist = strdup(new_data->artist);
        if (new_data->album)
            data->album = strdup(new_data->album);
        if (new_data->year)
            data->year = strdup(new_data->year);
        if (new_data->comment)
            data->comment = strdup(new_data->comment);
        data->track = new_data->track;

        // Handle genre
        if (new_data->genre)
        {
            data->genre = strdup(new_data->genre);
            data->genre_code = get_genre_code_by_name(new_data->genre);
        }
        else
        {
            data->genre_code = new_data->genre_code > 0 ? new_data->genre_code : 12; // Default to "Other"
        }

        // Write ID3v1 tags
        int result = write_id3v1_tags(filename, data);
        free_tag_data(data);

        return result == 0 ? e_success : e_failed;
    }
    else if (id3_version == 2)
    {
        // Handle ID3v2 tags
        // Determine ID3v2 version
        int subversion = 3; // Default to 2.3
        if (new_data->version && strncmp(new_data->version, "2.", 2) == 0)
        {
            int ver = new_data->version[2] - '0';
            if (ver == 2 || ver == 3)
            {
                subversion = ver;
            }
        }

        // Write ID3v2 tags
        return write_id3v2_tags(filename, new_data, subversion) == 0 ? e_success : e_failed;
    }
    else
    {
        display_error("Failed to determine ID3 version from filename");
        return e_failed;
    }
}