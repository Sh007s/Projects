/* Header file */
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
 
/* Function definition to read and validate arguments */

Status read_and_validate_encode_args(int argc,  char *argv[] , EncodeInfo *encInfo )
{
    if(strstr(argv[2], ".bmp") == NULL)
    {
	printf("Error: argv[2] is not passed properly.\n");
	printf("Usage: ./a.out -e beautifull.bmp secert.txt\n");
	return e_failure;
    }

    /* Checking whether passed image file is bmp file or not */
    if ( strcmp( strstr(argv[2] , ".bmp" ) , ".bmp" ) == 0 )
    {
	/* Storing passed bmp file */

	encInfo->src_image_fname = argv[2] ;
	printf("Yes! argv[2] is a .bmp file\n");
	if( argc != 3)
	{
    	    encInfo -> secret_fname = argv[3];
	    if(argv[4] != NULL)
    	    {
		printf("Yes! argv[4] passed\n");
		if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
		{
		    /* Storing passed output file */
		    printf("image name ");
		    encInfo->stego_image_fname = argv[4] ;
		}
		else
		{
		    printf("Error\n");		
		    return e_failure;
		}
	    }
	    else
	    {
    		/* If output file is not passed then Creating new file with name stego_image.bmp */
    		encInfo->stego_image_fname = "stego.bmp" ;
    	    }
	}
	else
	{
	    printf("only filename.bmp is file passed\n");
	    return e_failure;
	}
	return e_success ;
    }
    else
    {
	printf("Error! argv[2] is not a .bmp file\n");
    }
}

/* Function definition for do encoding */
Status do_encoding ( EncodeInfo *encInfo )
{

    /* Function call for open files function and checking condition whether success or not */
    if(  open_files(encInfo) == e_success )
    {

	printf ("Open files is a success\n");

	/* Function call to check capacity function and checking whether condition is success or not */
	if ( check_capacity(encInfo) == e_success )
	{

	    printf ( "Check capacity is a success \n");

	    /* Function call for copying bmp header function and checking condition whether success or not */
	    if ( copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success )
	    {

		printf ("Copied bmp header successfully \n");

		/* Function call to encode magic string  and checking whether condition is success or not */
		if ( encode_magic_string(MAGIC_STRING, encInfo) == e_success )
		{

		    printf ( "Encoded magic string \n");

		    /* Copying secret file extention */
		    strcpy ( encInfo->extn_secret_file , strstr(encInfo->secret_fname , "." ) );

		    /* Function call for encode secret file extention size  and checking condition whether success or not */
		    if ( encode_secret_file_extn_size ( strlen (encInfo->extn_secret_file), encInfo->fptr_src_image , encInfo->fptr_stego_image)  == e_success )
		    {
			printf ("Encoded secret file extn size \n");

			/* Function call encode secret file extention function and checking condition whether success or not */
			if ( encode_secret_file_extn( encInfo->extn_secret_file , encInfo ) == e_success )
			{
			    printf ("Encoded secret file successfully \n");

			    /* Function call for encode secret file size function and checking condition whether success or not */
			    if ( encode_secret_file_size(encInfo->size_secret_file , encInfo ) == e_success )
			    {
				printf ("Encoded secret file size succsfully\n");

				/* Function call for encode secret file data function and checking condition whether success or not */
				if ( encode_secret_file_data( encInfo ) == e_success )
				{
				    printf ("Encoded secret file data \n");

				    /* Function call for copying remianing image data function and checking condition whether success or not */
				    if ( copy_remaining_img_data( encInfo->fptr_src_image , encInfo->fptr_stego_image ) == e_success )
				    {
					printf ("Copied remaining data\n");
				    }
				    else
				    {
					printf ("Failed to copy remaining data\n");
					return e_failure ;
				    }
				}
				else
				{
				    printf ("Failes to encode secret file data \n");
				    return e_failure ;
				} 
			    }
			    else
			    {
				printf ("Failed to encode secret file size \n");
				return e_failure ;
			    }
			}
			else
			{
			    printf ("Failed to encode secret file extn \n");
			    return e_failure ;

			}
		    }
		    else
		    {
			printf ("Failed to encode scret file extn size \n");
			return e_failure ;
		    }
		}
		else
		{
		    printf ( "Failed to encode magic string \n");
		    return e_failure ;
		}
	    }
	    else
	    {

		printf ("Failed to copy bmp header \n");
		return e_failure ;
	    }
	}
	else
	{
	    printf ( " Check capacity is a failure \n");

	    return e_failure ;
	}
    }
    else
    {
	printf ("Open files is a failure\n");
	return e_failure;
    }
    return e_success ;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    /* Seek to 18th byte */
    fseek(fptr_image, 18, SEEK_SET);
    /* Read the width (an int) */
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    /*Read the height (an int) next 4 bytes from width */
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    /* Return image capacity */
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    /* Src Image file */
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r"); 
    /* Do Error handling */
    if (encInfo->fptr_src_image == NULL)
    {
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname); //printing error message.
	return e_failure;
    }
    /* Secret file */
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    /* Do Error handling */
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
	return e_failure;
    }
    /* Stego Image file */
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    /* Do Error handling */
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
	return e_failure;
    }
    /* No failure return e_success */
    return e_success;
}

/* Function definition for check capacity of beautiful.bmp file */
Status check_capacity(EncodeInfo *encInfo)
{
    /* Function call for get image size for bmp and storing return value */
    encInfo->image_capacity =  get_image_size_for_bmp( encInfo->fptr_src_image );

    /* Calling function get file size and stroing return value */
    encInfo->size_secret_file = get_file_size( encInfo->fptr_secret );

    /* Checking whether beautiful.bmp image file size is greater than secret file or not */
    if ( encInfo->image_capacity > ( (2 + 4 + 4 + 4 + encInfo->size_secret_file ) * 8 ) )
    {
	return e_success;
    }
    else
    {
	e_failure ;
    }
}

/* Function definition for get file size */
uint get_file_size( FILE *fptr )
{
    /* Seek file pointer to 0 */
    fseek(fptr , 0 , SEEK_END ); 
    return ftell(fptr);
}

/* Function definition for copying bmp file header to stego_image.bmp file */
Status copy_bmp_header( FILE *fptr_src_image , FILE *fptr_dest_image )
{
    char ptr[54] ;
    /* Seek file pointer to 0 */
    fseek(fptr_src_image, 0 ,SEEK_SET);
    /* Readin 54 bytes of src image to an array */
    fread(ptr , 54 , 1 , fptr_src_image);
    /* writing  54 bytes from array to destination i.e , stego file */
    fwrite(ptr , 54 , 1 , fptr_dest_image);
    return e_success;
}

/* Function definition for encoding magic string to output image file */
Status encode_magic_string( const char  *magic_string, EncodeInfo *encInfo )
{
    encode_data_to_image(magic_string , 2 , encInfo->fptr_src_image , encInfo->fptr_stego_image , encInfo );
    return e_success ;
}

/* Function definition for encoding data to output image file */
Status encode_data_to_image( const char *data , int size , FILE *fptr_src_image , FILE *fptr_stego_image , EncodeInfo *encInfo )
{
    for ( int i = 0 ; i < size ; i++ )
    {
	fread(encInfo->image_data , 8 , 1 , fptr_src_image );
	encode_byte_to_lsb(data[i] , encInfo->image_data );
	fwrite (encInfo->image_data , 8 , 1 , fptr_stego_image );
    }
}

/* Function definition for encoding each byte of magic string to lsb */
Status encode_byte_to_lsb( char data , char *image_buffer )
{
    unsigned int mask = 0x80 , i ;
    for ( i = 0 ; i < 8 ; i++ )
    {
	image_buffer[i]  = (image_buffer[i] & 0xFE ) | ( ( data & mask ) >> ( 7 - i ) );
	mask = mask >> 1 ;
    }
}

/* Function definition for encoding secret file extension size output image file */
Status encode_secret_file_extn_size( int size , FILE *fptr_src_image , FILE *fptr_stego_image )
{
    char str[32] ;
    fread(str , 32 , 1 , fptr_src_image );
    encode_size_to_lsb ( size , str );
    fwrite(str , 32 ,1 , fptr_stego_image );
    return e_success;
}

/* Function definition for encoding size to output image file */
Status encode_size_to_lsb( int size , char *image_buffer )
{
    unsigned int mask = 1 << 31 , i ;
    for ( i = 0 ; i < 32 ; i++ )
    {
	image_buffer[i]  = (image_buffer[i] & 0xFE ) | ( ( size & mask ) >> ( 31 - i ) );
	mask = mask >> 1 ;
    }
}

/* Function definition for encoding secret  file extention to output image file */
Status encode_secret_file_extn( char *file_extn , EncodeInfo *encInfo )
{
    encode_data_to_image (file_extn , strlen(file_extn) , encInfo->fptr_src_image , encInfo->fptr_stego_image , encInfo);
    return e_success ;
}

/* Function definition for encoding secret file size */
Status encode_secret_file_size( int size , EncodeInfo *encInfo )
{
    char str[32] ;
    fread(str , 32 , 1 , encInfo->fptr_src_image );
    encode_size_to_lsb( size , str );
    fwrite(str , 32 ,1 , encInfo->fptr_stego_image );
    return e_success;
}

/* Function definition for encoding secret file data to output image file */
Status encode_secret_file_data( EncodeInfo *encInfo )
{
    fseek ( encInfo->fptr_secret , 0 , SEEK_SET );
    char str[encInfo->size_secret_file ];
    fread ( str , encInfo->size_secret_file , 1 , encInfo->fptr_secret );
    encode_data_to_image(str , strlen(str) , encInfo->fptr_src_image , encInfo->fptr_stego_image , encInfo );
    return e_success ;
}

/* Function definition for copying remaining data of beautiful.bmp file to output image file */
Status copy_remaining_img_data( FILE *fptr_src , FILE *fptr_dest )
{
    char ch ;
    while ( ( fread ( &ch , 1 , 1 , fptr_src)) > 0 )
    {
	fwrite ( &ch , 1 , 1 , fptr_dest );
    }
    return e_success ;
} 
