#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("Width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("Height = %u\n", height);
     printf("\n");

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen"); //it will show error in the fopen 
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}



Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
  /* 1. check argv[2] == NULL
     yes -> printf(".bmp file not found)
     return failure  */
         if(argv[2] == NULL)
         {
         printf("The .bmp file not found\n");
         return e_failure;
         }
         
         /*  2. check strstr(argv[2] == NULL) 
  yes  ->  printf("Invalid image file name") 
           return failure
           */
           if(strstr(argv[2], ".bmp") == NULL)
           {
           printf("Invalid Image File Name\n");
           return e_failure;
           }
           
 // 3. enInfo -> src_image_fname = argv[2];
   
      encInfo->src_image_fname = argv[2];
   /*
  4. check argv[3] == NULL
          yes-> printf(".txt file no found");
          return failure;
          */
          if(argv[3] == NULL)
          {
          printf("The .txt File not found\n");
          return e_failure;
          }
          /*
  5. check strstr(argv[3], ".txt") == NULL
        yes-> print invalid sec file name
                return failure
                */

        if(strstr(argv[3], ".txt") == NULL)
        {
            printf("Invalid secret file name\n");
            return e_failure;
        }
                
 // 6. encInfo->sec_fname = argv[3]
  
      encInfo->secret_fname = argv[3];
  /*
  7. check argv[4] == NULL
        yes-> encInfo->stego_image_fname = "stego.bmp"
        no->  validate and store to encInfo->stego_image_fname = argv[4]
        */
        if(argv[4] == NULL)
        {
            encInfo->stego_image_fname = "stego.bmp";
        }
        else
        {
            encInfo->stego_image_fname = argv[4];
        }
        /*
  8. copy sec file extn to encInfo->extn_sec_file array
            1. char *chr = strchar(encInfo->sec_fname, '.')
            2. strcpy(encInfo->extn_sec_file, chr);
            */
            char *chr; 
            chr = strchr(encInfo->secret_fname, '.');
            strcpy(encInfo->extn_secret_file, chr);
            
          return e_success; // return success
  
  
}


Status do_encoding(EncodeInfo *encInfo)
{
    
   /* ret = open_files(enInfo)
    2. check ret = failure
    yes -> printf("File open failed");
    return failure   //  to the do encoding then do encoding check it then it return to main function
    */

     Status ret = open_files(encInfo);

        if(ret == e_failure)
        {
            printf("File open failed\n");
            return e_failure;
        }

   // 3. ret =  check_capacity(encInfo)
   // 4.check (ret == e_failure)
   // return e_failure
   ret =  check_capacity(encInfo); 
    if(ret == e_failure)
     {
      return e_failure;
     }

   // 5. copy_bmp_header(enInfo->fptr_src_image, enInfo->fptr_stego_image)
    ret = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(ret == e_failure)
    {
        return e_failure;
    }

     fseek(encInfo->fptr_src_image, 54, SEEK_SET);
     
    // 6. ret = encode_magic_string(MAGIC_STRING, encInfo);
    // return success or failure.
     ret = encode_magic_string(MAGIC_STRING, encInfo);
     if(ret == e_failure)
     {
       return e_failure;
     }
     
     // 7. check encode_secret_file_extn_size(strlen(file_extn_size), encInfo);  
     ret = encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
     if(ret == e_failure)
     {
        return e_failure;
     }
     
     // 8. ret = encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
     ret = encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
     if(ret == e_failure)
     {
        return e_failure;
     }

     
     // 9. calculate secret file size and  -->make sure to set offset back to first byte
     fseek(encInfo->fptr_secret, 0, SEEK_END);
     encInfo->size_secret_file = ftell(encInfo->fptr_secret);
     fseek(encInfo->fptr_secret, 0, SEEK_SET);

     // 9.  ret = encode_secret_file_size((encInfo->size_secret_file), encInfo);
     ret = encode_secret_file_size((encInfo->size_secret_file), encInfo);
     if(ret == e_failure)
     {
        return e_failure;
     }
      
      // 10. ret =  encode_secret_file_data(encInfo);
      ret = encode_secret_file_data(encInfo);
      if(ret == e_failure)
      {
        return e_failure;
      }
      
      // 11.copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
      ret = copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image); 
      if(ret == e_failure)
      {
        return e_failure;
      }
      //12.close 3 files
      // return success
      return e_success; 
}


Status check_capacity(EncodeInfo *encInfo)
{
    /*
   1.calculate no. of bytes needed for encoding from src file
            count = (magic_str_len +
            extn_size(int) + 
            extn_len + 
            file_size(int) + 
            file_data_len ) * 8 + 54
            
    2. check count <= src_file_size
        yes-> return sucess
        no-> return failure
        */
        int image_size;
        int encode_size;
        int secret_file_size;

       image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
       fseek(encInfo->fptr_src_image, 0, SEEK_SET);
       fseek(encInfo->fptr_secret, 0, SEEK_END);
       secret_file_size = ftell(encInfo->fptr_secret);
       rewind(encInfo->fptr_secret);

        encode_size = (strlen(MAGIC_STRING)+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(int)+secret_file_size)*8+54;
        printf("Total bytes : %d\n", encode_size);
         printf("\n");
        if((encode_size-54) <= image_size)
        {
            return e_success;
        }
        else 
        {
            return e_failure;
        }  
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    /*
    1. char temp[55];
    2. fread(fptr_src_image, 54, 1, temp); // for reading header
    3. fwrite(fptr_dest_image, 54, 1, temp) // writting header
        return success;
    */
    unsigned char temp[54];
    fread(temp, 1, 54, fptr_src_image);
    fwrite(temp, 1, 54, fptr_dest_image);
    printf("Copying of header is successfully!....\n");
    printf("After copying header, offset at %ld\n", ftell(fptr_dest_image));
     printf("\n");
    return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    /*
    1. run a loop
       for(int i=1; i<=2; i++)
       {
       char temp[8];
       1. read 8 bytes buffer from src file and store to temp array
       2. byte_to_lsb(magic_string[i], temp);
       3. write temp array 8 bytes to stego_file.
       }
    */
    for(int i=0; magic_string[i] != '\0'; i++)
    {
       char temp[8];
       fread(temp, 1, 8, encInfo->fptr_src_image);
       encode_byte_to_lsb(magic_string[i], temp);
       fwrite(temp, 1, 8, encInfo->fptr_stego_image);
    }
    printf("Magic string encoded successfully!......\n");
     printf("\n");
    return e_success;
}


 Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo)
 {
 /*
 char temp_buffer[32];
 1. read 32 bytes buff from src file
 2. call the function size_to_lsb(file_extn_size, temp_buffer);
 3. write temp_buffer to stego_file
*/
  char temp_buffer[32];
  fread(temp_buffer, 1, 32, encInfo->fptr_src_image);
  encode_byte_to_lsb_size(file_extn_size, temp_buffer);
  fwrite(temp_buffer, 1, 32, encInfo->fptr_stego_image);
 // printf("%s\n", encInfo->encode_secret_file_extn);
  printf("Secret file extension size encoded successfully!....\n");
   printf("\n");
  return e_success;
 }
 

 Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
 {
    /*
     1. run a loop 0 to file_extn_size-1 times
    {
       1. char temp[8];
       read 8 bytes of buffer from src file;
       call byte_to_lsb(file_extn[i], temp);
       4. write 8 bytes temp to stego file
    } */
       for(int i=0; i<strlen(file_extn); i++)
       {
       char temp[8];
       fread(temp, 1, 8, encInfo->fptr_src_image);
       encode_byte_to_lsb(file_extn[i], temp);
       fwrite(temp, 1, 8, encInfo->fptr_stego_image);
       }
       printf("Secret file extension encoded successfully!....\n");
        printf("\n");
       return e_success;
 }

 Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
       /*
        1. char temp[32];
        2. read 32 bytes of buff from src file
        3. cal size_to_lsb(file_size, temp);
        4. write temp to stego file
        */
   char temp[32];
   fread(temp, 1, 32, encInfo->fptr_src_image);
   encode_byte_to_lsb_size(file_size, temp);
   fwrite(temp, 1, 32, encInfo->fptr_stego_image);

   printf("Secret file size encoded successfully!....\n");
    printf("\n");
   return e_success;
}



Status encode_secret_file_data(EncodeInfo *encInfo)
{
    /*
        1. run a loop until src_file reaching EOF
        {
            1. read a ch from sec file
            2. read 8 bytes buff from src file
            3.call byte_to_lsb(ch, temp)
            4.write temp to stego file
        }
            */
        int ch;
        char temp[8];

   
       while((ch = fgetc(encInfo->fptr_secret)) != EOF)
       {

        fread(temp, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
       }

    printf("Secret file encoded successfully!.......\n");
     printf("\n");
    return e_success;
    
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    /*
        1. run a loop until src file is reaching EOF
        {
            1. read a 1 byte from src file
            2.write 1 byte to dest file
        }
    */
      int ch;
      while((ch= fgetc(fptr_src_image)) != EOF)
      {
        fputc(ch, fptr_stego_image);
      // fwrite(&ch, 1, 1, fptr_stego_image);
      }
      printf("Remaining image data copied successfully!.....\n");
      printf("\n");
      return e_success;
    
}
        
void encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
        char mask = 1<<(7-i);
        char bit = data & mask;

        image_buffer[i] = image_buffer[i] & 0xFE;

        bit =(unsigned)bit >> (7-i);
        image_buffer[i] = image_buffer[i] | bit;
      
    }
}

void encode_byte_to_lsb_size(int data, char *image_buffer)
{
    for(int i=0; i<32; i++)
    {
        int mask = 1<<(31-i);
        unsigned int bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xFE;
        bit = bit >> (31-i);
        image_buffer[i] = image_buffer[i] | bit;
     
    }
}
