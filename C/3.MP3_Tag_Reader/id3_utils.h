#ifndef ID3_UTILS_H
#define ID3_UTILS_H

typedef struct {
  char *version; /**< Version of the ID3 tag */
  char *title;   /**< Title of the song */
  char *artist;  /**< Artist of the song */
  char *album;   /**< Album name */
  char *year;    /**< Year of release */
  char *comment; /**< Comment */
  char *genre;   /**< Genre */
                 // Add other fields as needed
} TagData;

/**
 * @brief Creates a new TagData structure.
 *
 * @return Pointer to the newly created TagData structure.
 */
TagData *create_tag_data();

void free_tag_data(TagData *data);

#endif // ID3_UTILS_H
