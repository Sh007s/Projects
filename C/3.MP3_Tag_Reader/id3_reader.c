/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include "id3_reader.h"
#include "error_handling.h"
#include "main.h"

// Supported Korean character encodings
static const char *korean_encodings[] = {
    "EUC-KR",
    "CP949",
    "UHC",
    "JOHAB",
    NULL};

// ID3v1 Genre list
static const char *genres[] = {
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

static unsigned int decode_synchsafe(const unsigned char bytes[4])
{
    return ((bytes[0] & 0x7f) << 21) |
           ((bytes[1] & 0x7f) << 14) |
           ((bytes[2] & 0x7f) << 7) |
           (bytes[3] & 0x7f);
}

static int is_korean_text(const unsigned char *buf, size_t len)
{
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

static char *convert_encoding(const char *input, size_t input_len, const char *from_charset)
{
    if (!input || input_len == 0 || !from_charset)
    {
        return NULL;
    }

    iconv_t cd = iconv_open("UTF-8", from_charset);
    if (cd == (iconv_t)-1)
    {
        return NULL;
    }

    size_t out_len = input_len * 4;
    char *output = malloc(out_len + 1);
    if (!output)
    {
        iconv_close(cd);
        return NULL;
    }

    char *input_ptr = (char *)input;
    char *output_ptr = output;
    size_t input_left = input_len;
    size_t output_left = out_len;

    if (iconv(cd, &input_ptr, &input_left, &output_ptr, &output_left) != (size_t)-1)
    {
        *output_ptr = '\0';
        iconv_close(cd);
        return output;
    }

    free(output);
    iconv_close(cd);
    return NULL;
}

static char *try_korean_encodings(const char *input, size_t input_len)
{
    for (int i = 0; korean_encodings[i] != NULL; i++)
    {
        char *result = convert_encoding(input, input_len, korean_encodings[i]);
        if (result)
        {
            return result;
        }
    }
    return NULL;
}

static char *get_frame_content(const char *buffer, int size)
{
    if (size <= 1)
    {
        return NULL;
    }

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
            {
                return result;
            }
        }
        return convert_encoding(buffer + 1, size - 1, "ISO-8859-1");
    }

    case 1:
    { // UTF-16 with BOM
        int bom_offset = 1;
        const char *encoding_name = "UTF-16";

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
        return convert_encoding(buffer + bom_offset, size - bom_offset, encoding_name);
    }

    case 2: // UTF-16BE without BOM
        return convert_encoding(buffer + 1, size - 1, "UTF-16BE");

    case 3:
    { // UTF-8
        result = malloc(size);
        if (result)
        {
            memcpy(result, buffer + 1, size - 1);
            result[size - 1] = '\0';
        }
        return result;
    }

    default:
        return NULL;
    }
}

static char *get_comment_content(const char *buffer, int size)
{
    if (size <= 4)
    {
        return NULL;
    }

    unsigned char encoding = buffer[0];
    const char *description_start = buffer + 4;
    int desc_len = 0;

    switch (encoding)
    {
    case 0: // ISO-8859-1
    case 3: // UTF-8
        while (desc_len < size - 4 && description_start[desc_len] != 0)
        {
            desc_len++;
        }
        if (desc_len < size - 4)
        {
            return strdup(description_start + desc_len + 1);
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
            return strdup(description_start + desc_len + 2);
        }
        break;
    }

    return NULL;
}

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
        display_error("Not a valid ID3 tag");
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

    unsigned int tag_size = decode_synchsafe(header.size);
    char frame_id[5] = {0};
    unsigned char size_bytes[4];
    unsigned char flags[2];
    char *buffer = NULL;

    while (ftell(file) < tag_size + 10)
    {
        if (fread(frame_id, 1, 4, file) != 4)
            break;
        if (frame_id[0] == 0)
            break;

        if (fread(size_bytes, 1, 4, file) != 4 ||
            fread(flags, 1, 2, file) != 2)
        {
            break;
        }

        unsigned int frame_size = decode_synchsafe(size_bytes);
        if (frame_size == 0 || frame_size > MAX_FRAME_SIZE)
        {
            fseek(file, frame_size, SEEK_CUR);
            continue;
        }

        buffer = realloc(buffer, frame_size);
        if (!buffer)
            break;

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

        // Store frame content in tag_data
        if (strncmp(frame_id, "TIT2", 4) == 0)
            tag_data->title = content;
        else if (strncmp(frame_id, "TPE1", 4) == 0)
            tag_data->artist = content;
        else if (strncmp(frame_id, "TALB", 4) == 0)
            tag_data->album = content;
        else if (strncmp(frame_id, "TYER", 4) == 0)
            tag_data->year = content;
        else if (strncmp(frame_id, "COMM", 4) == 0)
            tag_data->comment = content;
        else if (strncmp(frame_id, "TCON", 4) == 0)
            tag_data->genre = content;
        else
            free(content);
    }

    free(buffer);
    fclose(file);

    // Set version string
    char version_str[16];
    snprintf(version_str, sizeof(version_str), "ID3v2.%d.%d",
             header.version[0], header.version[1]);
    tag_data->version = strdup(version_str);

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
    data->comment = strndup(tag + 97, 30);

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

    printf("\nMetadata:\n");
    printf("----------------------------------------\n");
    printf("Version : %s\n", data->version ? data->version : "Unknown");
    printf("Title   : %s\n", data->title ? data->title : "Unknown");
    printf("Artist  : %s\n", data->artist ? data->artist : "Unknown");
    printf("Album   : %s\n", data->album ? data->album : "Unknown");
    printf("Year    : %s\n", data->year ? data->year : "Unknown");
    printf("Comment : %s\n", data->comment ? data->comment : "Unknown");
    printf("Genre   : %s\n", data->genre ? data->genre : "Unknown");
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
