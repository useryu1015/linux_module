#include <SM3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
 
# define EVP_MAX_MD_SIZE                 32
#define ALLOC_OBJ(type, size) (type *)calloc(1, (sizeof(type)*size))
 
 
int main()
{
        unsigned char msgbuf[] = "abc";
        unsigned char msgbuf1[] = "abc";
  unsigned char dgst[EVP_MAX_MD_SIZE];
  unsigned int dgstlen = (unsigned int)sizeof(dgst);
 
 
  SM3_state *ctx = ALLOC_OBJ(SM3_state, 1);
  SM3_Init(ctx);
  SM3_Update(ctx, msgbuf, sizeof(msgbuf)-1);
  SM3_Final(dgst, dgstlen, ctx, sizeof(msgbuf)-1);
 
 
   printf("Digest1 is: ");
   for (int i = 0; i < dgstlen; i++)
          printf("%02x", dgst[i]);
   printf("\n");
 
 
        // second use
        //    SM3_Hash(msgbuf1 , sizeof(msgbuf1)-1, dgst, dgstlen);
        //       printf("Digest2 is: ");
        //          for (int i = 0; i < dgstlen; i++)
        //               printf("%02x", dgst[i]);
        //                  printf("\n");
        //                   
        //                    
        //                     
        //                      
        //                         free(ctx);
        //                           return 0;
        //                           }
        //
