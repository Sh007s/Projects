/* Header files */
#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function definition of read and validate function */
Status read_and_validate_decode_args( char *argv[] , DecodeInfo *decInfo )
{
    if ( strcmp( strstr(argv[2] , "." ) , ".bmp" ) == 0 )
    {
	decInfo->stego_image_fname = argv[2] ;
	
	if ( argv[3] != NULL )
    	{
    	    decInfo->decode_fname = argv[3] ;
    	}
    	else
    	{
	decInfo->decode_fname = "output" ;
	}
    }
    else
    {
	return e_failure ;
    }
    return e_success ;
}

Status do_decoding( DecodeInfo *decInfo )
{
    /* opening files for decoding */
    if ( open_file( decInfo ) == e_success )
    {
	printf ("Open files is a success \n");

	/* Decoding magic string */
	if ( decode_magic_string( MAGIC_STRING , decInfo->magic , decInfo ) == e_success )
	{
	    printf ("Decoded magic string successfully\n");

	    /*Decoding file extension size */
	    if ( decode_secret_file_extn_size( decInfo , decInfo->fptr_stego_image ) == e_success )
	    {
		printf ("Decoded file extn size successfully\n");

		/* Decoding file extension */
		if ( decode_secret_file_extn( decInfo -> file_extn_size , decInfo) == e_success )
		{
		    printf ("Decoded file extn successfully\n");
		    char file[100];
		    strcpy(file, decInfo -> decode_fname );
		    strcat(file, decInfo -> file_extn);
		 //   printf("%s\n", file);
		    decInfo -> decode_fname = file;
		   // printf("%s\n", decInfo -> decode_fname);

		    decInfo -> fptr_decode = fopen (decInfo -> decode_fname, "w");
		    if(decInfo -> fptr_decode == NULL)
		    {
			perror("fopen");
			fprintf(stderr, "Error: Unable to open file %s\n", decInfo -> decode_fname);
		    return e_failure;
		    }

		    /* Decoding file size */
		    if ( decode_secret_file_size ( decInfo ) == e_success )
		    {
			printf ("Decoded file size from stego image file successfully \n");

			/* Decoding data from image */
			if ( decode_data_from_image ( decInfo-> file_size , decInfo ) == e_success )
			{
			    printf ("Decoded data from image successfully\n");
			}
			else
			{
			    printf ("Failed to decode data from image \n");
			    return e_failure ;
			}
		    }
		    else
		    {
			printf ("Failed to decode file size from stego image  file \n" );
			return e_failure ;
		    }
		}
		else
		{
		    printf ("Failed to decode file extn \n");
		    return e_failure ;
		} 
	    }
	    else
	    {
		printf ("Failed to decode file extn size \n");
		return e_failure ;
	    }
	}
	else
	{
	    printf ("Failed to decode magic string\n");
	    return e_failure ;
	}
    }
    else
    {
	printf ("Failed to open files\n");
	return e_failure ;
    }
    return e_success ;
}

/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo)
{
    /* Stego Image file */
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    /* Error handling */
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }
  /*  No failure return e_success */
    return e_success;
}

/* Decoding magic string */
Status decode_magic_string( const char *magic_string , char *magic , DecodeInfo *decInfo )
{
    char data[8] ;
    /* Setting stego file pointer at 54 byte position */
    fseek (decInfo->fptr_stego_image , 54 , SEEK_SET );
    /* Decoding magic string from lsb, storing it to an string and comparing both and returning ouput */

    for ( int i  = 0 ; i < strlen(magic_string ) ; i++ )
    {
	/* Reading 8 bytes once from stego_image_pointer to data */
	fread( data , 8 , 1 , decInfo->fptr_stego_image );
	magic[i] = decode_byte_from_lsb(data);
    }
    if ( strcmp(magic_string , magic) == 0 )
    {
	return e_success ;
    }
    else
    {
	return e_failure ;
    }
}

/* Function definition for file extension size */
Status decode_secret_file_extn_size( DecodeInfo *decInfo , FILE *fptr_stego_image )
{
    char arr[32];
    fread( arr, 32 , 1, fptr_stego_image);
    decInfo->file_extn_size = decode_size_to_lsb(arr);
//    printf("%d\n", decInfo -> file_extn_size);
    return e_success;
}

/* Function definition for file extension */
Status decode_secret_file_extn(uint file_extn_size, DecodeInfo *decInfo)
{
    char arr[8];
    int i;
    char data;
    for(i = 0 ; i < file_extn_size ; i++)
    {
	fread( arr ,8, 1, decInfo -> fptr_stego_image);
	data = decode_byte_from_lsb(arr);
	decInfo -> file_extn[i] = data;
    } 
    decInfo -> file_extn[i] = '\0';
  //  printf("%s\n", decInfo -> file_extn);
    return e_success;
}

/* Function definition to decode secret file size from stego file*/
Status decode_secret_file_size( DecodeInfo *decInfo )
{     
    char arr[32]; 
    fread(arr, 32, 1, decInfo -> fptr_stego_image);
    decInfo -> file_size = decode_size_to_lsb(arr);
    return e_success;
}

/* Function definition to decode data from image */
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
    char arr[8];
    for( int i = 0 ; i < size ; i++)
    {
	fread(arr, 8, 1, decInfo -> fptr_stego_image);
	decInfo -> decode_data[0] = decode_byte_from_lsb(arr);
	fwrite(decInfo -> decode_data , 1 , 1 , decInfo->fptr_decode);
    }
    return e_success;
}

/* Function definition to decode each byte of lsb */
char decode_byte_from_lsb(char *data_buffer)
{
    char data = 0;
    for(int i = 0 ; i < 8 ; i++)
    {
	data_buffer[i] = data_buffer[i] & 1;
	data = (data << 1) | data_buffer[i];
    }
    return data;
}

uint decode_size_to_lsb( char *buffer )
{
    unsigned int  data =  0;
    for ( int i = 0 ; i < 32 ; i++ )
    {
	buffer[i] = buffer[i] & 1 ;
	data =  ( data << 1 ) | buffer[i] ;
    }
    return data ;
}
