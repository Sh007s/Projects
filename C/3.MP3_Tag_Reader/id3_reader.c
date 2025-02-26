/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include "id3_reader.h"
#include "error_handling.h"
#include "main.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// Supported Korean character encodings
const char *korean_encodings[] = {
    "EUC-KR",
    "CP949",
    "UHC",
    "JOHAB",
    NULL};

// ID3v1 Genre list
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

/**
 * @brief Converts a synchsafe integer used in ID3 frames to a regular integer
 */
unsigned int decode_synchsafe(const unsigned char bytes[4])
{
    return ((bytes[0] & 0x7f) << 21) |
           ((bytes[1] & 0x7f) << 14) |
           ((bytes[2] & 0x7f) << 7) |
           (bytes[3] & 0x7f);
}

/**
 * @brief Detects if text contains Korean encoding
 */
unsigned int is_korean_text(const unsigned char *buf, size_t len)
{
    if (!buf || len < 2)
        return 0;

    for (size_t i = 0; i < len - 1; i++)
    {
        if ((buf[i] == 0xEA || buf[i] == 0xEB) &&
            (buf[i + 1] >= 0x80 && buf[i + 1] <= 0xBF))
        {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Cleans up a string by removing common artifacts
 */
void clean_string(char *str)
{
    if (!str)
        return;

    // Remove website references and clean hyphens
    static const char *patterns[] = {
        "MassTamilan.dev",
        "PenduJatt.Com",
        "PenduJatt.net",
        "SenSongsMp3.Com",
        "::",
        " -", // Space before hyphen
        "- ", // Space after hyphen
        NULL};

    // First pass: Remove website patterns
    for (int i = 0; patterns[i]; i++)
    {
        char *found = strstr(str, patterns[i]);
        if (found)
        {
            *found = '\0';
        }
    }

    // Second pass: Clean up multiple spaces and trim
    char *read = str;
    char *write = str;
    int space = 0;

    while (*read)
    {
        // Skip multiple spaces
        if (isspace(*read))
        {
            if (!space)
            {
                *write++ = ' ';
                space = 1;
            }
        }
        else
        {
            *write++ = *read;
            space = 0;
        }
        read++;
    }
    *write = '\0';

    // Trim trailing whitespace
    size_t len = strlen(str);
    while (len > 0 && isspace(str[len - 1]))
    {
        str[--len] = '\0';
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && isspace(*start))
        start++;

    if (start != str)
    {
        memmove(str, start, strlen(start) + 1);
    }
}

/**
 * @brief Converts text from one encoding to another
 */
char *convert_encoding(const char *input, size_t input_len, const char *from_charset)
{
    if (!input || input_len == 0 || !from_charset)
        return NULL;

    iconv_t cd = iconv_open("UTF-8", from_charset);
    if (cd == (iconv_t)-1)
        return NULL;

    size_t out_len = input_len * 4;
    char *output = calloc(1, out_len + 1);
    if (!output)
    {
        iconv_close(cd);
        return NULL;
    }

    char *input_ptr = (char *)input;
    char *output_ptr = output;
    size_t input_left = input_len;
    size_t output_left = out_len;

    size_t result = iconv(cd, &input_ptr, &input_left, &output_ptr, &output_left);
    iconv_close(cd);

    if (result == (size_t)-1)
    {
        SAFE_FREE(output);
        return NULL;
    }

    return output;
}

/**
 * @brief Tries multiple Korean encodings
 */

char *try_korean_encodings(const char *input, size_t input_len)
{
    if (!input || input_len == 0)
        return NULL;

    for (int i = 0; korean_encodings[i] != NULL; i++)
    {
        char *result = convert_encoding(input, input_len, korean_encodings[i]);
        if (result && strlen(result) > 0)
            return result;
        SAFE_FREE(result);
    }
    return NULL;
}

char *normalize_genre(const char *raw_genre)
{
    if (!raw_genre || strlen(raw_genre) == 0)
    {
        return strdup("Unknown");
    }

    char *result = NULL;
    char *temp = strdup(raw_genre);
    if (!temp)
    {
        return strdup("Unknown");
    }

    // Convert to lowercase for easier comparison
    for (char *p = temp; *p; p++)
    {
        *p = tolower(*p);
    }

    // Handle numeric genre codes (ID3v1 standard)
    if (temp[0] == '(' && isdigit(temp[1]))
    {
        int genre_num = atoi(temp + 1);
        const int num_genres = sizeof(genres) / sizeof(genres[0]);
        if (genre_num >= 0 && genre_num < num_genres)
        {
            result = strdup(genres[genre_num]);
        }
        else if (genre_num == 255)
        {
            char *paren_end = strchr(temp, ')');
            result = (paren_end && strlen(paren_end) > 1) ? strdup(paren_end + 1) : strdup("Custom");
        }
        else
        {
            result = strdup("Unknown");
        }
        free(temp);
        return result;
    }

    // Handle specific known artist names in genre field
    if (strcmp(temp, "thaman s") == 0)
    {
        free(temp);
        return strdup("Telugu");
    }

    // Remove website domains and common non-genre text
    if (strstr(temp, ".com") || strstr(temp, ".org") || strstr(temp, ".net") ||
        strstr(temp, "www.") || strstr(temp, "http"))
    {
        const char *languages[] = {"tamil", "telugu", "hindi", "malayalam", "kannada"};
        const char *results[] = {"Tamil Song", "Telugu", "Hindi", "Malayalam", "Kannada"};
        const int langs_count = sizeof(languages) / sizeof(languages[0]);

        for (int i = 0; i < langs_count; i++)
        {
            if (strstr(temp, languages[i]))
            {
                free(temp);
                return strdup(results[i]);
            }
        }
        free(temp);
        return strdup("Unknown");
    }

    // Check for Indian language music genres
    const char *indian_languages[] = {"tamil", "telugu", "hindi", "malayalam", "kannada", "marathi", "punjabi", "bengali"};
    const int num_languages = sizeof(indian_languages) / sizeof(indian_languages[0]);
    for (int i = 0; i < num_languages; i++)
    {
        if (strstr(temp, indian_languages[i]))
        {
            char *capitalized = strdup(indian_languages[i]);
            capitalized[0] = toupper(capitalized[0]);

            if (strstr(temp, "song"))
            {
                char *combined = malloc(strlen(capitalized) + 6); // +6 for " Song" and null terminator
                if (combined)
                {
                    sprintf(combined, "%s Song", capitalized);
                    free(capitalized);
                    free(temp);
                    return combined;
                }
            }
            free(temp);
            return capitalized;
        }
    }

    // Handle common film music abbreviations
    if (strcmp(temp, "ost") == 0 || strstr(temp, "soundtrack") || strstr(temp, "original sound"))
    {
        free(temp);
        return strdup("OST");
    }

    // Trim whitespace
    char *start = temp;
    char *end = temp + strlen(temp) - 1;
    while (isspace(*start))
        start++;
    while (end > start && isspace(*end))
        *end-- = '\0';

    result = (strlen(start) == 0) ? strdup("Unknown") : strdup(start);
    free(temp);
    return result;
}

/**
 * @brief Extracts content from ID3v2 frame
 */

char *get_frame_content(const char *buffer, int size)
{
    if (!buffer || size <= 1)
        return NULL;

    unsigned char encoding = buffer[0];
    char *result = NULL;

    switch (encoding)
    {
    case 0:
    { // ISO-8859-1 or potential Korean
        if (is_korean_text((const unsigned char *)buffer + 1, size - 1))
        {
            result = try_korean_encodings(buffer + 1, size - 1);
            if (result)
                return result;
        }
        result = convert_encoding(buffer + 1, size - 1, "ISO-8859-1");
        break;
    }
    case 1:
    { // UTF-16 with BOM
        const char *encoding_name = "UTF-16";
        int bom_offset = 1;

        if (size > 3)
        {
            if ((unsigned char)buffer[1] == 0xFF && (unsigned char)buffer[2] == 0xFE)
            {
                encoding_name = "UTF-16LE";
                bom_offset = 3;
            }
            else if ((unsigned char)buffer[1] == 0xFE && (unsigned char)buffer[2] == 0xFF)
            {
                encoding_name = "UTF-16BE";
                bom_offset = 3;
            }
        }
        result = convert_encoding(buffer + bom_offset, size - bom_offset, encoding_name);
        break;
    }
    case 2: // UTF-16BE without BOM
        result = convert_encoding(buffer + 1, size - 1, "UTF-16BE");
        break;
    case 3:
    { // UTF-8
        result = calloc(1, size);
        if (result)
        {
            memcpy(result, buffer + 1, size - 1);
        }
        break;
    }
    }

    if (result)
    {
        clean_string(result);
    }
    return result;
}

/**
 * @brief Extracts content from ID3v2 comment frame
 */
char *get_comment_content(const char *buffer, int size)
{
    if (!buffer || size <= 4)
        return NULL;

    unsigned char encoding = buffer[0];
    const char *description_start = buffer + 4;
    int desc_len = 0;
    char *comment_text = NULL;

    // Skip language bytes (3 bytes) and find description terminator
    switch (encoding)
    {
    case 0: // ISO-8859-1
    case 3: // UTF-8
        while (desc_len < size - 4 && description_start[desc_len])
            desc_len++;
        if (desc_len < size - 4)
        {
            comment_text = convert_encoding(description_start + desc_len + 1,
                                            size - desc_len - 5,
                                            encoding == 0 ? "ISO-8859-1" : "UTF-8");
        }
        break;

    case 1: // UTF-16 with BOM
    case 2: // UTF-16BE
        while (desc_len < size - 5 &&
               !(description_start[desc_len] == 0 && description_start[desc_len + 1] == 0))
        {
            desc_len += 2;
        }
        if (desc_len < size - 5)
        {
            comment_text = convert_encoding(description_start + desc_len + 2,
                                            size - desc_len - 6,
                                            encoding == 1 ? "UTF-16" : "UTF-16BE");
        }
        break;
    }

    return comment_text;
}

/**
 * @brief Reads ID3v2 tags from an MP3 file
 */
TagData *read_id3v2_tags(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        display_error("Cannot open file");
        return NULL;
    }

    struct ID3v2_header header;
    if (fread(&header, 1, sizeof(header), file) != sizeof(header))
    {
        display_error("Cannot read ID3 header");
        fclose(file);
        return NULL;
    }

    if (strncmp(header.identifier, "ID3", 3) != 0)
    {
        fclose(file);
        return NULL; // Not an error, just no ID3v2 tag
    }

    int id3_version = header.version[0];
    if (id3_version == 4)
    {
        display_error("ID3v2.4 is not supported");
        fclose(file);
        return NULL;
    }

    unsigned int tag_size = decode_synchsafe(header.size);
    if (tag_size == 0 || tag_size > 100000000)
    { // Sanity check for tag size
        display_error("Invalid tag size");
        fclose(file);
        return NULL;
    }

    TagData *tag_data = calloc(1, sizeof(TagData));
    if (!tag_data)
    {
        display_error("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char frame_id[5] = {0};
    unsigned char size_bytes[4];
    unsigned char flags[2];
    char *buffer = NULL;
    long file_pos;

    // Initialize track values
    tag_data->track = 0;
    tag_data->total_track = 0;

    while ((file_pos = ftell(file)) >= 0 && file_pos < tag_size + 10)
    {
        int frame_id_size = (id3_version == 2) ? 3 : 4;
        if (fread(frame_id, 1, frame_id_size, file) != frame_id_size)
            break;
        frame_id[frame_id_size] = '\0';

        // Break if we reach a null frame ID
        if (frame_id[0] == 0)
            break;

        int frame_size_bytes = (id3_version == 2) ? 3 : 4;
        if (fread(size_bytes, 1, frame_size_bytes, file) != frame_size_bytes)
            break;

        // Read flags
        if (fread(flags, 1, 2, file) != 2)
            break;

        // Calculate frame size
        unsigned int frame_size;
        if (id3_version == 3)
        {
            frame_size = (size_bytes[0] << 24) | (size_bytes[1] << 16) |
                         (size_bytes[2] << 8) | size_bytes[3];
        }
        else
        {
            frame_size = decode_synchsafe(size_bytes);
        }

        if (frame_size == 0 || frame_size > MAX_FRAME_SIZE)
        {
            if (fseek(file, frame_size, SEEK_CUR) != 0)
                break;
            continue;
        }

        // Allocate buffer for frame content
        char *new_buffer = realloc(buffer, frame_size);
        if (!new_buffer)
        {
            SAFE_FREE(buffer);
            break;
        }
        buffer = new_buffer;

        if (fread(buffer, 1, frame_size, file) != frame_size)
            break;

        char *content = NULL;
        if (strncmp(frame_id, "COMM", 4) == 0)
        {
            content = get_comment_content(buffer, frame_size);
        }
        else
        {
            content = get_frame_content(buffer, frame_size);
        }

        if (!content)
            continue;

        //   printf("DEBUG: Processing frame_id: '%s' with content: '%s'\n", frame_id, content);

        // Handle different frame types
        if (strncmp(frame_id, "TRCK", 4) == 0)
        {
            int track_num = 0, total_track = 0;

            // Skip encoding byte if needed
            char *track_text = content;

            // Clean non-printable characters
            char cleaned_content[20] = {0};
            strncpy(cleaned_content, track_text, sizeof(cleaned_content) - 1);

            for (size_t i = 0; i < strlen(cleaned_content); i++)
            {
                if (!isprint(cleaned_content[i]))
                {
                    cleaned_content[i] = '\0';
                    break;
                }
            }

            // Parse track number
            if (sscanf(cleaned_content, "%d/%d", &track_num, &total_track) >= 1)
            {
                tag_data->track = track_num;
                tag_data->total_track = (total_track > 0) ? total_track : 0;
            }
            else if (sscanf(cleaned_content, "%d", &track_num) == 1)
            {
                tag_data->track = track_num;
            }

            free(content);
        }
        else if (strncmp(frame_id, "TIT2", 4) == 0 || strncmp(frame_id, "TT2", 3) == 0)
        {
            SAFE_FREE(tag_data->title);
            tag_data->title = content;
        }
        else if (strncmp(frame_id, "TPE1", 4) == 0 || strncmp(frame_id, "TP1", 3) == 0)
        {
            SAFE_FREE(tag_data->artist);
            tag_data->artist = content;
        }
        else if (strncmp(frame_id, "TALB", 4) == 0 || strncmp(frame_id, "TAL", 3) == 0)
        {
            SAFE_FREE(tag_data->album);
            tag_data->album = content;
        }
        else if (strncmp(frame_id, "TYER", 4) == 0 || strncmp(frame_id, "TDRC", 4) == 0 ||
                 strncmp(frame_id, "TYE", 3) == 0)
        {
            SAFE_FREE(tag_data->year);
            tag_data->year = content;
        }
        else if (strncmp(frame_id, "COMM", 4) == 0)
        {
            SAFE_FREE(tag_data->comment);
            tag_data->comment = content;
        }
        else if ((strncmp(frame_id, "TCON", 4) == 0) && (strncmp(frame_id, "TCO", 3) == 0))
        {
            //      printf("DEBUG: Processing frame_id: '%s'\n", frame_id);
            SAFE_FREE(tag_data->genre);
            //     printf("DEBUG: Raw Genre content: '%s'\n", content);
            char *normalized_genre = normalize_genre(content);
            //    printf("DEBUG: Normalized Genre: '%s'\n", normalized_genre);
            free(content);
            tag_data->genre = normalized_genre;
        }
        else
        {
            free(content);
        }
    }

    SAFE_FREE(buffer);

    // Set version string
    char version_str[16];
    snprintf(version_str, sizeof(version_str), "2.%d.%d", header.version[0], header.version[1]);
    tag_data->version = strdup(version_str);

    fclose(file);
    return tag_data;
}

/**
 * @brief Reads ID3v1 tags from an MP3 file
 */
TagData *read_id3v1_tags(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        display_error("Failed to open file");
        return NULL;
    }

    if (fseek(fp, -TAG_SIZE, SEEK_END) != 0)
    {
        display_error("Error seeking file");
        fclose(fp);
        return NULL;
    }

    char tag[TAG_SIZE];
    if (fread(tag, sizeof(char), TAG_SIZE, fp) != TAG_SIZE ||
        strncmp(tag, "TAG", 3) != 0)
    {
        fclose(fp);
        return NULL;
    }

    TagData *data = calloc(1, sizeof(TagData));
    if (!data)
    {
        display_error("Failed to allocate memory for TagData");
        fclose(fp);
        return NULL;
    }

    data->version = strdup("ID3v1");
    data->title = strndup(tag + 3, 30);
    data->artist = strndup(tag + 33, 30);
    data->album = strndup(tag + 63, 30);
    data->year = strndup(tag + 93, 4);

    if (tag[125] == 0)
    {
        data->comment = strndup(tag + 97, 28); // Only 28 bytes for comment in ID3v1.1
        data->track = (unsigned char)tag[126]; // Track number
        data->version = strdup("ID3v1.1");
    }
    else
    {
        data->comment = strndup(tag + 97, 30); // Full 30 bytes for comment in ID3v1
        data->track = 0;                       // No track number
    }

    // Handle genre
    unsigned char genre_code = tag[127];
    if (genre_code < sizeof(genres) / sizeof(genres[0]))
    {
        data->genre = strdup(genres[genre_code]);
    }
    else
    {
        data->genre = strdup("Unknown");
    }

    fclose(fp);
    return data;
}

/**
 * @brief Displays metadata in a formatted way
 */
void display_metadata(const TagData *data)
{
    if (!data)
    {
        display_error("No metadata available");
        return;
    }

    printf("\n   MP3 Tag Reader & Editor:\n");
    printf("\n----------------------------------------\n");
    printf("\nVersion  ID : %s\n", data->version ? data->version : "Unknown");
    printf("Title       : %s\n", data->title ? data->title : "Unknown");
    printf("Album       : %s\n", data->album ? data->album : "Unknown");
    printf("Year        : %s\n", data->year ? data->year : "Unknown");
    printf("Track       : %d\n", data->track);
    printf("Genre       : %s\n", data->genre ? data->genre : "Unknown");
    printf("Artist      : %s\n", data->artist ? data->artist : "Unknown");
    printf("Comment     : %s\n\n", data->comment ? data->comment : "Unknown");

    printf("Extracting Album Art - Done\n");
    printf("----------------------------------------\n");
}

/**
 * @brief Reads and displays ID3 tags from a file
 */
void view_tags(const char *filename)
{
    // Try ID3v2 first
    TagData *data = read_id3v2_tags(filename);

    // Fall back to ID3v1 if ID3v2 fails
    if (!data)
    {
        data = read_id3v1_tags(filename);
    }

    if (data)
    {
        display_metadata(data);
        free_tag_data(data);
    }
    else
    {
        display_error("No ID3 tags found in file");
    }
}