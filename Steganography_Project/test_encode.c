#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "encode.h"
#include "types.h"

int main(int argc, char *argv[])   //this main function file common for both encode and decode
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret =  check_operation_type(argv); 

   if(ret == e_unsupported)
  {
    printf("Invalid args\n");
    return e_unsupported;
  }

  if(ret == e_encode)
  {
   // encoding
   int ret =  read_and_validate_encode_args(argv, &encInfo);
   if(ret == e_failure)
   {
    printf("Invalid args\n");
    return e_failure;
   }
   
   // start encoding
   ret = do_encoding(&encInfo);
   if(ret == e_failure)
   {
    printf("Encoding failed\n");
    return e_failure;
   }
   else
   {
    printf("........Encoding is successfull.......\n");
    printf("\n");
    return e_success;
   }


  }


  if(ret == e_decode)
  {
    // decoding
   int ret =  read_and_validate_decode_args(argv, &decInfo);
   if(ret == e_failure)
   {
    printf("Invalid args\n");
    return e_failure;
   }

   // start decoding
   ret = do_decoding(&decInfo);
   if(ret == e_failure)
   {
    printf("Decoding failed\n");
    return e_failure;
   }
   else
   {
    printf(".......Decoding is successfull!.......\n");
    printf("\n");
    return e_success;
   }
  }
  
}
  

OperationType check_operation_type(char *argv[]) 
{
    /*
    1. check argv[1] == NULL
    yes -> return e_unsupported
    */
    if(argv[1] == NULL)
    {
      return e_unsupported;
    }
    /*
    2. (strcmp(argv[1], "-e") == 0)
    yes -> return e_encode
    */
    if(strcmp(argv[1], "-e") == 0)
    {
      return e_encode;
    }

    /*
    3. (strcmp(argv[1], "-d") == 0)
    yes -> return e_decode
    */
    if(strcmp(argv[1], "-d") == 0)
    {
      return e_decode;
    }
    
    return e_unsupported;
}  