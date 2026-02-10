#include <stdio.h>
#include <string.h>
#include "types.h"
#include "decode.h"

Status open_decode_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego == NULL)
    {
    	perror("fopen"); //it will show error in the fopen 
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_fname);

    	return e_failure;
    }

    return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
     
     // check stego.bmp
    if(argv[2] == NULL)
    {
        printf(".bmp  File is Not Passed");
        return e_failure;
    }

    if((strstr(argv[2],".bmp")) == NULL)
    {
        printf("Invalid Image File Name");
        return e_failure;
    }
   // store in decInfo-> fname
   decInfo->stego_fname = argv[2];
    
    if(argv[3] == NULL)
    {
        //store default name for output file to default
        decInfo->dest_fname = "Output";   
    }
    else
    {
     decInfo->dest_fname = argv[3];
    }
     return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    
  //  1. open stego file
   Status ret = open_decode_files(decInfo);

        if(ret == e_failure)
        {
            printf("File Open Failed\n");
            return e_failure;
        }


   // 2. call skip_bmp_header(decInfo->fptr_stego)
   //  check using ftell if the offset is present at the 54
    ret = skip_bmp_header(decInfo->fptr_stego);
    if(ftell(decInfo->fptr_stego) != 54)
    {
        printf("Failed to Skip BMP header\n");
        return e_failure;
    }

   // 3. declare one empty array --> char magic_string[50];
   // 4. decode_magic_string(decInfo->fptr_stego, magic_string);
    char magic_string[50];
    ret = decode_magic_string(decInfo->fptr_stego, magic_string);
    if(ret == e_failure)
    {
        return e_failure;
    } 

   // 5. read magic string from the user
   /*6. compare user entered and decode magic strings are same ?;
    if no-> err: invalid magic string
    return failure  */
    char user[20];
    printf("Enter the MAGIC STRING :");
    scanf("%s", user);
    if(strcmp(user, magic_string) != 0)
    {
        printf("ERROR: Invalid magic string!\n");
        return e_failure;
    }


   // 7. int extn_size;
   // 8. decode_extn_file_size(decInfo->fptr_stego, &extn_size);  
    int extn_size;
    ret = decode_extn_file_size(decInfo->fptr_stego, &extn_size);
    if(ret == e_failure)
    {
    return e_failure;
    }


  //  9. char extn[10];
  //  10. decode_extn_file(decInfo->fptr_stego, &extn, extn_size)
    char extn[10];
    ret = decode_extn_file(decInfo->fptr_stego, extn, extn_size);
    if(ret == e_failure)
    {
    return e_failure;
    }


   // 11. concatenate output file name and decoded  extn
   // 12. open output file
   // 13. int file_size
  // printf("extn : %s\n", extn);
   strcpy(decInfo->output_fname, decInfo->dest_fname);
   strcat(decInfo->output_fname, extn);
  // printf("%s\n", decInfo->output_fname);
   printf("Output file and extension concatenated successfully!.....\n");

   decInfo->fptr_dest = fopen(decInfo->output_fname, "w");
   if(decInfo->fptr_dest == NULL)
   {
    perror("Error : File not created!\n");
    return e_failure;
   }
   printf("Output file created successfully!....\n");
    printf("\n");


   //14. decode_secret_file_size(decInfo->fptr_stego, &file_size)
     int file_size;
    ret = decode_secret_file_size(decInfo->fptr_stego, &file_size);
    if(ret == e_failure)
    {
    return e_failure;
    }


   // 15. decode_secret_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size)
   // char extn[10];
    ret = decode_secret_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size);
    if(ret == e_failure)
    {
    return e_failure;
    }


   // 16. close files
   // 17. return success
   return e_success;   
}


Status skip_bmp_header(FILE *fptr_stego)
{
    fseek(fptr_stego, 54, SEEK_SET);
    printf("Successfulyy skipped the bmp header!...\n");
    printf("After skipping bmp header then offset at %ld\n", ftell(fptr_stego));
    printf("\n");
    return e_success;
}


Status decode_magic_string(FILE *stego, char *magic_string)
{
    /* for(int i=0; i<2; i++)
    {  
    char temp[8];
    1. read 8 bytes buffer from stego file 
    2. call magic_string[i] = lsb_to_byte(buffer);
    } */
    for(int i=0; i<2; i++)
    {
        char temp[8];
        fread(temp, 1, 8, stego);
        magic_string[i] = lsb_to_byte(temp);
    }
    magic_string[2] = '\0';
    printf("Magic string decoded successfully!.....\n");
    printf("After magic string decoded then offset at %ld\n", ftell(stego));
    printf("\n");
    return e_success;
}


char lsb_to_byte(char *buffer) // it takes only buffer and returns the decode data of 8 bytes
{
    /*
    1. run a loop for 8 times
      get lsb of each index of buffer and store to ith position of data variable
      return data
    */
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        char bit;
        bit = buffer[i] & 1;
        bit = bit << (7-i);
        data = data | bit;
    }
    return data;
}


int lsb_to_byte_size(char *buffer)
{
    int data = 0;
    for (int i = 0; i < 32; i++)
    {
        int bit;
        bit = buffer[i] & 1;        
        bit = bit << (31 - i);     
        data = data | bit;         
    }
    return data;
}


Status decode_extn_file_size(FILE *stego, int *extn_size)
{
 
  //  1.read 32 bytes buffer
  //  2.*extn_size = lsb_to_size(buffer)   
 
    char temp[32];
    fread(temp, 32, 1, stego);
    *extn_size = lsb_to_byte_size(temp);
    printf("Extension file size decoded successfully!....\n");
    printf("After extension file size decoded then offset at %ld\n", ftell(stego));
     printf("\n");
    // printf("%d\n", *extn_size);
    return e_success;
}

Status decode_extn_file(FILE *stego, char *extn, int extn_size)
{
    /* run a loop for 0 to extn_size-1
    1. read 8 bytes temp buffer;
    2. extn[i] = lsb_to_byte(temp) */
    char temp[8];
    int i;
    for(i=0; i<=extn_size-1; i++)
    {  
        fread(temp, 8, 1, stego);
        extn[i] = lsb_to_byte(temp);
    }
    extn[i] = '\0';
   // printf(" %s\n", extn);
     printf("Extension file decoded successfully!....\n");
     printf("After extension decoded then offset at %ld\n", ftell(stego));
     printf("\n");
     return e_success;  
}


Status decode_secret_file_size(FILE *stego, int *file_size)
{
     /* 1.read 32 bytes buffer
        2.*fie_size = lsb_to_size(buffer);  */
    char temp[32];
    fread(temp, 1, 32, stego);
    *file_size = lsb_to_byte_size(temp);
     printf("Secret file size decoded successfully!....\n");
     printf("After secret file extension decoded then offset at  %ld\n", ftell(stego));
    // printf("Secret file size : %d\n", *file_size);
     printf("\n");
     return e_success;
}


Status decode_secret_data(FILE *stego, FILE *dest, int file_size)
{
     /*   run a loop for file_size times 
    1.read 8 byte temp buffer;
    2.char ch =lsb_to_byte(temp_buffer);
    3. write ch to dest file */
   
     for(int i=0; i<file_size; i++)
    {
         char temp[8];
        fread(temp, 1, 8, stego);
        char ch = lsb_to_byte(temp);
        fwrite(&ch, 1, 1, dest);
    }
     printf("Secret file decoded successfully!....\n");
     printf("After secret file decoded then offset at %ld\n", ftell(stego));
     printf("\n");
     return e_success;
}
     
