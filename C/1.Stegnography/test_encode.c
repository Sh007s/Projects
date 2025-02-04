/* Documentation

 * Description : C-project on Stegnography.
 * Name        : SHANKAR S
 * Date        : 28/12/2022
 * Description :
 * Input       : 1.For Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp
 *               2.For Decoding: ./a.out -d stege.bmp decode_msg.txt
 * Output      : 1
 *               ----------Choosen Encoding part----------
 *
 * 		Read and Validate encode arguments is  a success
 * 		<---------------Started Encoding-------------->
 *               Open files is a success
 *               width = 1024
 *		height = 768
 *		Check capacity is a success
 *               Copied bmp header successfully
 *		The Magic string is encoded
 *		Encoded secret file extn size
 *		Encoded secret file extn successfully
 *		Secret file size is encoded successfully
 *		Encoded secret file data
 *		Copied remaining data
 *
 *		**************Encoded successfully**************
 *
 *               2
 *	        <------------Choosen Decoding part-------------->
 *
 *		Read and validate decode arguments is a success
 *		<-------------Started Decooding----------------->
 *		Open files is a success
 *		Decoded Magic string successfully
 *		Decoded File Extension size successfully
 *		Decoded Extension successfully
 *		Decoded File size successfully
 *		Decoded File data Successfully
 *
 *		************Decoded Successfully****************
 */


/* Including headers */
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"


/* Main function with arguments entered by the user through command line */

int main( char argc , char **argv )

{
    /* Checking whether required filenames are passed or not */

    if ( argc < 3 || argc > 5)
    {
	printf("Encodeong is not possible please pass arguments in between 3 and 5\n");
	printf("Usage: .Please pass for Encoding: ./a.out -e beautiful.bmp secret.txt stego_image.bmp\n");
	return e_failure;
    }
    else if ( check_operation_type( argv ) == e_encode)
    {	    
	printf ("\n ----------Choosen Encoding part----------\n");
	printf("\nI am going to do Encoding \n");
	EncodeInfo encInfo ;
	/* Reading and validating encode arguments */
	if (read_and_validate_encode_args( argc, argv , &encInfo ) == e_success )
	{
	    printf ("Read and validate encode arguments is a success \n");
	    printf ("<--------------------Started Encoding-------------------->\n");
		/* Encode function call */
		if ( do_encoding( &encInfo ) == e_success )
		{
		    printf ("\n*****************Encoded Successfully*******************\n");
		}
		else
		{
		    printf ("Failed to encode\n");
		    return -1 ;
		}
	    }
	    else
	    {
		printf ("Read and validate encode arguments is a failure \n");
		return e_failure; ;
	    }
	    return 0;
	}
	/* Validation if OperationType is decoding */
    else if ( check_operation_type(argv) == e_decode )
    {
	if( argc < 3 || argc > 4 )
    	{
	printf("Encodeong is not possible please pass arguments in between 3 and 4\n");
	printf("Usage: .Please pass for Decoding: ./a.out -d stego.bmp\n");
	return e_failure;
	}
	else
	{
	    printf ("\n!!....................Selected Decoding..................!!\n");
    	    printf("\nI am going to do Decoding \n");
	    DecodeInfo decInfo ;
	    /* Reading and validating decode arguments */
	    if (read_and_validate_decode_args( argv , &decInfo ) == e_success )
	    {
		printf ("Read and validate decode arguments is a success \n");
		printf ("<--------------------Started Decoding-------------------->\n");
		/* Decode function call */
		if ( do_decoding( &decInfo ) == e_success )
		{
		    printf ("\n*****************Decoded Successfully********************\n");
		}
		else
		{
		    printf ("Failed to decode\n");
		    return -1 ;
		}	    }
	    else
	    {
		printf ("Read and validate decode arguments is a failure \n");
		return -1 ;
	    }
	}
    }
    else if( check_operation_type(argv) == e_unsupported)
    {
	printf("Error: please pass valid type of operation\n");
	printf("Usage: ./a.out -e beautifull.bmp secret.txt\nUsage: ./a.out. -d stego.bmp\n");
    }
    return 0;
} 

/*Function definition to check the Operation type*/
OperationType check_operation_type( char *argv[] )
{
    if ( strcmp(argv[1] , "-e" )  == 0 )
    {
	return e_encode ;
    }
    if ( strcmp( argv[1] , "-d" ) == 0 )
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}
