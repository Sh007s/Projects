/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include "id3_reader.h"
#include "error_handling.h"
#include "main.h"

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

// New helper functions for string cleanup
static void decode_html_entities(char *str)
{
    if (!str)
        return;

    char *write = str;
    char *read = str;

    while (*read)
    {
        if (strncmp(read, "&quot;", 6) == 0)
        {
            *write++ = '"';
            read += 6;
        }
        else if (strncmp(read, "&amp;", 5) == 0)
        {
            *write++ = '&';
            read += 5;
        }
        else if (strncmp(read, "&lt;", 4) == 0)
        {
            *write++ = '<';
            read += 4;
        }
        else if (strncmp(read, "&gt;", 4) == 0)
        {
            *write++ = '>';
            read += 4;
        }
        else
        {
            *write++ = *read++;
        }
    }
    *write = '\0';
}

static void clean_string(char *str)
{
    if (!str)
        return;

    // Remove website references and clean hyphens
    const char *patterns[] = {
        "MassTamilan.dev",
        "PenduJatt.Com",
        "PenduJatt.net",
        "SenSongsMp3.Com",
        "::",
        " -", // Add space before hyphen to avoid removing hyphens in legitimate names
        "- ", // Add space after hyphen
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
unsigned int decode_synchsafe(const unsigned char bytes[4])
{
    return ((bytes[0] & 0x7f) << 21) |
           ((bytes[1] & 0x7f) << 14) |
           ((bytes[2] & 0x7f) << 7) |
           (bytes[3] & 0x7f);
}

int is_korean_text(const unsigned char *buf, size_t len)
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

char *convert_encoding(const char *input, size_t input_len, const char *from_charset)
{
    if (!input || input_len == 0 || !from_charset)
        return NULL;

    iconv_t cd = iconv_open("UTF-8", from_charset);
    if (cd == (iconv_t)-1)
        return NULL;

    size_t out_len = input_len * 4;
    char *output = calloc(1, out_len + 1); // Use calloc to ensure null termination
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

TagData *read_id3v2_tags(const char *filename)
{
    printf("%s\n", filename);
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
        display_error("Not a valid ID3 tag");
        fclose(file);
        return NULL;
    }

    int id3_version = header.version[0];
    if (id3_version == 4)
    {
        display_error("ID3v2.4 is not Supported");
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
    tag_data->track = -1;

    while ((file_pos = ftell(file)) >= 0 && file_pos < tag_size + 10)
    {
        int frame_id_size = (id3_version == 2) ? 3 : 4;
        if (fread(frame_id, 1, frame_id_size, file) != frame_id_size)
            break;
        frame_id[frame_id_size] = '\0';
        if (frame_id[0] == 0)
            break;

        int frame_size_bytes = (id3_version == 2) ? 3 : 4;
        if (fread(size_bytes, 1, frame_size_bytes, file) != frame_size_bytes || fread(flags, 1, 2, file) != 2)
        {
            break;
        }

        unsigned int frame_size = decode_synchsafe(size_bytes);
        if (frame_size == 0 || frame_size > MAX_FRAME_SIZE)
        {
            if (fseek(file, frame_size, SEEK_CUR) != 0)
                break;
            continue;
        }

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

        // Store frame content

        if (strncmp(frame_id, "TRCK", 4) == 0)
        {
            // int track_num = atoi(content);
            // // Store values safely
            // tag_data->track = (track_num > 0) ? track_num : -1;
            // //   tag_data->total_track = (total_tracks > 0) ? total_tracks : -1;
            int track_num = 0, total_track = 0;
            sscanf(content, "%d/%d", &track_num, &total_track);
            tag_data->track = (track_num > 0) ? track_num : -1;
            SAFE_FREE(content);
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
        else if (strncmp(frame_id, "TYER", 4) == 0 || strncmp(frame_id, "TDRC", 4) == 0 || strncmp(frame_id, "TYE", 3) == 0)
        {
            SAFE_FREE(tag_data->year);
            tag_data->year = content;
        }
        else if (strncmp(frame_id, "COMM", 4) == 0)
        {
            SAFE_FREE(tag_data->comment);
            tag_data->comment = content;
        }
        else if (strncmp(frame_id, "TCON", 4) == 0 || strncmp(frame_id, "TCO", 3) == 0)
        {
            SAFE_FREE(tag_data->genre);
            tag_data->genre = content;
        }
        else
        {
            SAFE_FREE(content);
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
        data->comment = strndup(tag + 97, 28); // Full 30 bytes for comment in ID3v1
        data->track = -1;                      // No track number
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

void view_tags(const char *filename)
{
    // Try ID3v2 first
    TagData *data = read_id3v2_tags(filename);
    if (data)
    {
        display_metadata(data);
        free_tag_data(data);
    }

    // Fall back to ID3v1 if ID3v2 fails
    if (!data)
    {
        //    TagData *data  = read_id3v1_tags(filename);
        data = read_id3v1_tags(filename);
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
}
