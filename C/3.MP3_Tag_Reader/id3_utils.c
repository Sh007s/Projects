#include "id3_utils.h"
#include <stdlib.h>
#include <string.h>

TagData *create_tag_data()
{
  TagData *data = (TagData *)malloc(sizeof(TagData));
  if (data)
  {
    data->version = NULL;
    data->title = NULL;
    data->track = NULL;
    data->artist = NULL;
    data->album = NULL;
    data->year = NULL;
    data->comment = NULL;
    data->genre = NULL;
    // Initialize other fields as needed
  }
  return data;
}

void free_tag_data(TagData *data)
{
  if (data)
  {
    free(data->version);
    free(data->title);
    free(data->track);
    free(data->artist);
    free(data->album);
    free(data->year);
    free(data->comment);
    free(data->genre);
    // Free other fields as needed
    free(data);
  }
}
