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

static unsigned int decode_synchsafe(const unsigned char bytes[4]);
static int is_korean_text(const unsigned char *buf, size_t len);
static char *convert_encoding(const char *input, size_t input_len, const char *from_charset);
static char *try_korean_encodings(const char *input, size_t input_len);
static char *get_frame_content(const char *buffer, int size);
static char *get_comment_content(const char *buffer, int size);

#endif // ID3_READER_H
