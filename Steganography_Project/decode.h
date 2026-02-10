#ifndef DECODE_H
#define DECODE_H
#include "types.h"

typedef struct _DecodeInfo
{
    char *stego_fname; //it stores the stego.bmp
    FILE *fptr_stego;

    char *dest_fname;
    FILE *fptr_dest;
    char output_fname[50];


}DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Skip the bmp header upto  */
Status skip_bmp_header(FILE *fptr_stego);

/* Decode Magic String */
Status decode_magic_string(FILE *stego, char *magic_string);

/* Decode a LSB into byte of image data array */
char lsb_to_byte(char *buffer);

/* Decode a LSB into byte of size of image data array */
int lsb_to_byte_size(char *buffer);

/* Decode secret file extenstion size */
Status decode_extn_file_size(FILE *stego, int *extn_size);

/* Decode secret file extenstion */
Status decode_extn_file(FILE *stego, char *extn, int extn_size);

/* Decode secret file size */
Status decode_secret_file_size(FILE *stego, int *file_size);

/* Decode secret file data */
Status decode_secret_data(FILE *stego, FILE *dest, int file_size);


#endif


