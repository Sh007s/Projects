#ifndef DECODE_H
#define DECODE_H

/* Header files */

#include<stdio.h>
#include"common.h"
#ifndef TYPES
#define TYPES
#include "types.h" // Contains user defined types
#endif

/* 
 * Structure to store information required for
 * decoding stego  file to source Image to text file .
 * Info about output and intermediate data is
 * also stored

 */

#define MAX_DECODE_BUF_SIZE 1
#define MAX_DATA_BUF_SIZE (MAX_DECODE_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */

    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint file_size ;
    uint file_extn_size ;
    char decode_data[MAX_DATA_BUF_SIZE];
    char file_extn[MAX_FILE_SUFFIX];
    char magic[3];
    char secret_file_extn[5];
    /* Decode File Info */

    char *decode_fname;
    FILE *fptr_decode;

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo);

/* Decoding magic string  */
Status decode_magic_string(const char *magic_string , char *magic ,  DecodeInfo *decInfo);

/* Decoding secret file extenstion size */
Status decode_secret_file_extn_size( DecodeInfo *decInfo , FILE *fptr_stego_image );

/* Decode secret file extenstion */
Status decode_secret_file_extn( uint file_extn_size , DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image( int size,  DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb( char *data_buffer);

/* Decode size from lsb */
uint decode_size_to_lsb(  char *buffer );

#endif
