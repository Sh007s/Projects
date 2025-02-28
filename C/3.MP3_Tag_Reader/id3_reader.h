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

#define MIN(a, b) ((a) < (b) ? (a) : (b)) // Macro to get the minimum of two values
#define TAG_SIZE 128                      // Standard ID3v1 tag size
#define ID3V2_HEADER_SIZE 10              // ID3v2 header size in bytes
#define MAX_FRAME_SIZE 1048576            // 1MB max frame size for safety
#define MAX_STRING_LENGTH 1024            // Max length for text fields

// Safe memory release macro
#define SAFE_FREE(p)  \
    do                \
    {                 \
        if (p)        \
        {             \
            free(p);  \
            p = NULL; \
        }             \
    } while (0)

// External declaration for the genre list (defined in genre_utils.c)
extern const char *genres[];

/**
 * @brief Structure representing an ID3v2 header.
 */
struct ID3v2_header
{
    char identifier[3];       ///< "ID3" identifier
    unsigned char version[2]; ///< Major and minor version
    unsigned char flags;      ///< Flags for the tag
    unsigned char size[4];    ///< Size of the tag (synchsafe integer)
};

/**
 * @brief Reads ID3v1 tags from an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @return A pointer to a TagData structure containing the extracted metadata, or NULL on failure.
 */
TagData *read_id3v1_tags(const char *filename);

/**
 * @brief Reads ID3v2 tags from an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @return A pointer to a TagData structure containing the extracted metadata, or NULL on failure.
 */
TagData *read_id3v2_tags(const char *filename);

/**
 * @brief Displays metadata stored in a TagData structure.
 *
 * @param data A pointer to the TagData structure.
 */
void display_metadata(const TagData *data);

/**
 * @brief Reads and displays all available ID3 tags from an MP3 file.
 *
 * @param filename The name of the MP3 file.
 */
void view_tags(const char *filename);

/**
 * @brief Removes leading and trailing whitespace from a string.
 *
 * @param str The string to clean.
 */
void clean_string(char *str);

/**
 * @brief Attempts to decode a string using various Korean encodings (EUC-KR, CP949, UTF-8).
 *
 * @param input The input string.
 * @param input_len The length of the input string.
 * @return A newly allocated string with the converted encoding, or NULL if conversion fails.
 */
char *try_korean_encodings(const char *input, size_t input_len);

/**
 * @brief Extracts the content of a frame from an ID3v2 tag.
 *
 * @param buffer Pointer to the frame data.
 * @param size The size of the frame data.
 * @return A newly allocated string containing the frame content.
 */
char *get_frame_content(const char *buffer, int size);

/**
 * @brief Extracts comment content from an ID3v2 comment frame.
 *
 * @param buffer Pointer to the comment frame data.
 * @param size The size of the comment frame.
 * @return A newly allocated string containing the comment content.
 */
char *get_comment_content(const char *buffer, int size);

/**
 * @brief Normalizes the genre field from ID3 tags.
 *
 * @param raw_genre The raw genre string from the MP3 metadata.
 * @return A newly allocated string with the normalized genre.
 */
char *normalize_genre(const char *raw_genre);

/**
 * @brief Converts the encoding of a given text string.
 *
 * @param input The input string.
 * @param input_len The length of the input string.
 * @param from_charset The source encoding (e.g., "UTF-16LE", "ISO-8859-1").
 * @return A newly allocated string in UTF-8 encoding, or NULL if conversion fails.
 */
char *convert_encoding(const char *input, size_t input_len, const char *from_charset);

/**
 * @brief Decodes a synchsafe integer from ID3v2 format.
 *
 * @param bytes A 4-byte array representing a synchsafe integer.
 * @return The decoded integer value.
 */
unsigned int decode_synchsafe(const unsigned char bytes[4]);

/**
 * @brief Checks whether a given text buffer contains Korean characters.
 *
 * @param buf Pointer to the text buffer.
 * @param len Length of the buffer.
 * @return 1 if Korean text is detected, 0 otherwise.
 */
unsigned int is_korean_text(const unsigned char *buf, size_t len);

#endif // ID3_READER_H
