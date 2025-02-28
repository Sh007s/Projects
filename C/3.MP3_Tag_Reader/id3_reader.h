#ifndef ID3_READER_H
#define ID3_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <iconv.h>
#include <errno.h>
#include "id3_utils.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TAG_SIZE 128
#define ID3V2_HEADER_SIZE 10
#define MAX_FRAME_SIZE 1048576
#define MAX_STRING_LENGTH 1024
#define SAFE_FREE(p)  \
    do                \
    {                 \
        if (p)        \
        {             \
            free(p);  \
            p = NULL; \
        }             \
    } while (0)


extern const char *genres[];
struct ID3v2_header
{
    char identifier[3];
    unsigned char version[2];
    unsigned char flags;
    unsigned char size[4];
};

TagData *read_id3v1_tags(const char *filename);
TagData *read_id3v2_tags(const char *filename);
void display_metadata(const TagData *data);
void view_tags(const char *filename);
void clean_string(char *str);
char *try_korean_encodings(const char *input, size_t input_len);
char *get_frame_content(const char *buffer, int size);
char *get_comment_content(const char *buffer, int size);
char *normalize_genre(const char *raw_genre);
char *convert_encoding(const char *input, size_t input_len, const char *from_charset);
unsigned int decode_synchsafe(const unsigned char bytes[4]);
unsigned int is_korean_text(const unsigned char *buf, size_t len);

#endif // ID3_READER_H
