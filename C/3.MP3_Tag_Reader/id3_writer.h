#ifndef ID3_WRITER_H
#define ID3_WRITER_H

#include "id3_utils.h"

#define MAX_TAG_SIZE 1024 // Define an appropriate size

/**
 * @brief Writes the ID3 tags to an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return 0 on success, non-zero on failure.
 */
int write_id3_tags(const char *filename, TagData *data);

/**
 * @brief Determines the ID3 version of an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @return The ID3 version (e.g., 1 or 2), or -1 on failure.
 */
int determine_id3_version(const char *filename);

/**
 * @brief Encodes an integer value as a synchsafe integer (used in ID3v2 tags).
 *
 * @param value The integer value to encode.
 * @param bytes Array to store the resulting 4-byte encoded value.
 */
void encode_synchsafe(unsigned int value, unsigned char bytes[4]);

/**
 * @brief Creates an ID3v2 frame with the given data.
 *
 * @param id The 4-character frame identifier (e.g., "TIT2" for title).
 * @param data Pointer to the frame data.
 * @param data_size The size of the frame data in bytes.
 * @param output Buffer to store the resulting frame.
 * @param id3_version The ID3 version (e.g., 2.3 or 2.4).
 * @return The total size of the created frame, or -1 on failure.
 */
int create_id3v2_frame(const char *id, const unsigned char *data, int data_size, unsigned char *output, int id3_version);

/**
 * @brief Creates an ID3v2 text frame (e.g., for title, artist, album).
 *
 * @param id The 4-character frame identifier.
 * @param text The text to store in the frame.
 * @param output Buffer to store the resulting frame.
 * @param id3_version The ID3 version (e.g., 2.3 or 2.4).
 * @return The total size of the created frame, or -1 on failure.
 */
int create_text_frame(const char *id, const char *text, unsigned char *output, int id3_version);

/**
 * @brief Writes ID3v2 tags to an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @param id3_version The ID3 version to use (2.3 or 2.4).
 * @return 0 on success, non-zero on failure.
 */
int write_id3v2_tags(const char *filename, const TagData *data, int id3_version);

/**
 * @brief Writes ID3v1 tags to an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return 0 on success, non-zero on failure.
 */
int write_id3v1_tags(const char *filename, TagData *data);

/**
 * @brief Sets all available ID3 tags for an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param new_data Pointer to the new TagData structure containing updated tags.
 * @return 0 on success, non-zero on failure.
 */
int set_all_tags(const char *filename, const TagData *new_data);

/**
 * @brief Edits a specific tag in an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param tag The tag identifier (e.g., "TIT2" for title).
 * @param value The new value for the tag.
 * @return 0 on success, non-zero on failure.
 */
int edit_tag(const char *filename, const char *tag, const char *value);

#endif // ID3_WRITER_H
