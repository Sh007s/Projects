#ifndef ID3_READER_H
#define ID3_READER_H

#include <stdio.h>
#include "id3_utils.h"

struct ID3v2_header {
    char identifier[3];
    unsigned char version[2];
    unsigned char flags;
    unsigned char size[4];
};

TagData *read_id3v1_tags(const char *filename);

TagData *read_id3v2_tags(const char *filename);

void display_metadata(const TagData *data);

void view_tags(const char *filename);

#endif // ID3_READER_H
