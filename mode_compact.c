// Rodrigo Pintassilgo - 2191190
//Daniel Patricio - 2191195




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/stat.h>

#include <dirent.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1 //used to fix 'asprintf' warning
#endif

#include "debug.h"
#include "memory.h"
#include "cmdline.h"
#include "mode_compact.h"

#define DT_REG 8 //8 is the value for regular files, 
                //this will be used to check if any file inside a directory is indeed regular file

#define CHUNK_LEN 1024 // length of chunks that wil be used to read files in portions



//mode_1
void mode_1_compact(char *token , FILE* out, struct gengetopt_args_info args_info){

	//100% funcional (-m 1)
    long int sum=0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
				fptr = fopen(token,"rb"); //open file
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

				uint8_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				int seen[256] = {0}; //this array is use to save how many times a byte is in the file
                //since the max value of a byte is 255, we create an array with 256 slots (255 possible values + '\0')

                if(!args_info.output_given){
				    printf("%s:%ldbytes:", token, size_of_file);
                }
                if(args_info.output_given){
				    fprintf(out,"%s:%ldbytes:", token, size_of_file);
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        while ((bytes_read = fread(buffer, sizeof(uint8_t), CHUNK_LEN, fptr)) > 0) {
                            //this will read in chunks until the fread returns 0, that means that it reached EOF
                            //if bytes_read is higher than 0, it will loop. When it is 0, it won't
                            for (size_t i = 0; i < bytes_read; i++) {
                                //this will loop buffer that has the bytes
                                //however this inner loop only loops one chunk so
                                //we are using another while loop outside
                                //if (buffer[i] > -1 && buffer[i] < 256) {
                                    //if their values are regular between the acceptable range,
                                    //we will count them
                                    //for example: if buffer[1] has the byte 3, this will be used
                                    //to count how many times that choosen file has the byte 3
                                    //so seen[3]++  -> byte3 was seen one time seen[3]=1
                                    seen[buffer[i]]++;
                                //}
                            }
                        }


                        for (int j = 0; j < 256; j++) {
                            //now we will loop seen file and check which bytes have been
                            //found in the file. if the byte was not seen, we won't print it
                                if (seen[j] > 0) {
                                    if(args_info.output_given){
                                        fprintf(out,"%d", seen[j]);
                                    }
                                    if(!args_info.output_given){
                                        printf("%d", seen[j]);
                                    }

						            sum = sum + seen[j];
                                    //this will sum how many times all byte were seen
                                 }           
                            
                        }
            

				

                    if(!args_info.output_given){
                        printf(":%ld\n", sum);
                    }
                    if(args_info.output_given){
                        fprintf(out,":%ld\n", sum);
                    }

				sum = 0; //empty the sum variable for the next file



				fclose(fptr); //closing the file
                if(!args_info.dir_given){ //this will be used incase token is actually a token (dir not given)
                    token = strtok(NULL, ","); //since we're looping while(token != NULL)
                 }   /* token = NULL;  } */ //we want to make sure it wont loop forever so we need to make sure
                                            //in this case token = strtok(NULL, ",") will do the same thing as toke = NULL
                                            //that would stop the loop           


                if(args_info.dir_given){
                    break; //this will break here if dir was given
                }
				
			}
	
}
  
 void mode_1_discrete(char * token, FILE* out, struct gengetopt_args_info args_info,
                    char * token_discrete, char *string_discrete) { //COLOCAR SITE STRTOK_R

    char *rest;
    while (token != NULL) {
        
    FILE * fptr = NULL;
    int testingRead = 1;
    fptr = fopen(token, "rb");

    if (fptr == NULL) {
        ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
    }

    fseek(fptr, 0L, SEEK_END); //offset made from the end of the file
    long int size_of_file = ftell(fptr);//calculating the size of the file

    uint8_t buffer[size_of_file]; //bufer with the size of file

    fseek(fptr, 0L, SEEK_SET); //go to the beginning of the file

    if (testingRead == 0) {
        ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
    }
    
    int seen[256];

    if(!args_info.output_given){
            printf("freqCounter:'%s':%ld bytes\n", token, size_of_file);
    }else{
            fprintf(out,"freqCounter:'%s':%ld bytes\n", token, size_of_file);
    }


    for ((token_discrete = strtok_r(string_discrete,",",&rest)); token_discrete != NULL; token_discrete = strtok_r(NULL,",",&rest)) {

            
    if (atoi (token_discrete) < 256 && atoi (token_discrete) >= 0) {

        for (int i=0;i<256;i++) {
            seen[i]=0;
            for (int j=0;j<size_of_file;j++) {
                if (buffer[j] == i) {
                    seen[i]++;
                }
            }
            if (atoi (token_discrete) == i) {
                if(!args_info.output_given){   
                     printf("byte %03d:%d\n", atoi (token_discrete), seen[i]);
                }else{
                     fprintf(out,"byte %03d:%d\n", atoi (token_discrete), seen[i]);
                }
            }
    
        }                
                        
    }
    }
    if(!args_info.output_given){
        printf("------------------\n");
    }else{
        fprintf(out,"------------------\n");
    }

    fclose(fptr);
    token = strtok(NULL, ",");
    }
}


void mode_1(char *token , FILE* out, struct gengetopt_args_info args_info){
    //this function isn't as explained as mode_1_compact since it does the same thing
    //the only difference between this one and compact is that they 'printf' different stuff

	//100% funcional (-m 1)
    long int sum=0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
				fptr = fopen(token,"rb"); //open file
            //fptr = fgets()
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

				uint8_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				int seen[256] = {0};

                if(!args_info.output_given){
				    printf("freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }
                if(args_info.output_given){
				    fprintf(out,"freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        while ((bytes_read = fread(buffer, sizeof(uint8_t), CHUNK_LEN, fptr)) > 0) {
                            for (size_t i = 0; i < bytes_read; i ++) {
                                //if (buffer[i] > -1 && buffer[i] < 256) {
                                    seen[buffer[i]]++;
                               // }
                            }
                        }


                        for (int j = 0; j < 256; j++) {
                                if (seen[j] > 0) {
                                    if(!args_info.output_given){
					                    printf("byte %03d:%d\n", j, seen[j]);
                                    }       
                                    if(args_info.output_given){
                                        fprintf(out,"byte %03d:%d\n", j, seen[j]);
                                    }

						            sum = sum + seen[j];
					            }          
                            
                        }
            

				

                if(!args_info.output_given){
				    printf("sum:%ld\n", sum);
                }
                if(args_info.output_given){
				    fprintf(out,"sum:%ld\n", sum);
                }

				sum = 0; //empty the sum variable for the next file
                if(!args_info.output_given){
				    printf("---------------\n");
                }
                if(args_info.output_given){
				    fprintf(out,"---------------\n");
                }



				fclose(fptr); //closing the file
                if(!args_info.dir_given){
                    token = strtok(NULL, ",");
                }


                if(args_info.dir_given){
                    break;
                }
				
			}
	
}



//mode_2
void mode_2_compact(char *token , FILE* out, struct gengetopt_args_info args_info){
    //this function does almost the same thing as mode_1_compact but now it's counting
    //two bytes instead of one

    long int sum=0;
    int notMultiple = 0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
				fptr = fopen(token,"rb"); //open file
            //fptr = fgets()
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

                //I'm using uint16_t since 16bits are equal to 2bytes and it will make it easier for us
				uint16_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				int seen[65536] = {0}; // now the value is 65536 since
                            //2bytes go grom 0 to 65536

                if(!args_info.output_given){
				    printf("%s:%ldbytes:", token, size_of_file);
                }
                if(args_info.output_given){
				    fprintf(out,"%s:%ldbytes:", token, size_of_file);
                }

                if( (size_of_file % 2) != 0){
                    //printf("The size of '%s' is not multiple of '2'!",token);
                    notMultiple = size_of_file % 2;
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        //same logic but now the loop is bigger
                        while (  (bytes_read = fread(buffer, sizeof(uint16_t), CHUNK_LEN, fptr)) > 0) {
                            for (size_t i = 0; i < bytes_read; i ++) {
                               // if (buffer[i] > -1 && buffer[i] < 65536) {
                                    seen[buffer[i]]++;
                               // }
                            }
                        }


                        for (int j = 0; j < (65536-notMultiple); j++) {
                                if (seen[j] > 0) {
                                    if(args_info.output_given){
                                        fprintf(out,"%d", seen[j]);
                                    }
                                    if(!args_info.output_given){
                                        printf("%d", seen[j]);
                                    }

						            sum = sum + seen[j];
                                 }           
                            
                        }
            

				

                    if(!args_info.output_given){
                        printf(":%ld\n", sum);
                    }
                    if(args_info.output_given){
                        fprintf(out,":%ld\n", sum);
                    }

				sum = 0; //empty the sum variable for the next file



				fclose(fptr); //closing the file
                if(!args_info.dir_given){
                    token = strtok(NULL, ",");
                }


                if(args_info.dir_given){
                    break;
                }
				
			}
	
}
 
void mode_2_discrete(char * token, FILE* out, struct gengetopt_args_info args_info, 
                                    char * token_discrete,char *string_discrete) { 

    char *rest;
    while (token != NULL) {
        
    FILE * fptr = NULL;
    int testingRead = 1;
    fptr = fopen(token, "rb");

    if (fptr == NULL) {
        ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
    }

    fseek(fptr, 0L, SEEK_END); //offset made from the end of the file
    long int size_of_file = ftell(fptr);//calculating the size of the file

    uint16_t buffer[size_of_file/2]; //bufer with the size of file
    //WARNING: This is completly wrong and we know it
    //We're using an old function that was partially working
    //since our recent one(without vla) was doing less prints....

    fseek(fptr, 0L, SEEK_SET); //go to the beginning of the file

    if (testingRead == 0) {
        ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
    }
    
    int seen[65536];

    if(!args_info.output_given){
            printf("freqCounter:'%s':%ld bytes\n", token, (size_of_file/2));
    }else{
            fprintf(out,"freqCounter:'%s':%ld bytes\n", token, (size_of_file/2));
    }


    for ((token_discrete = strtok_r(string_discrete,",",&rest)); token_discrete != NULL; token_discrete = strtok_r(NULL,",",&rest)) {

            
    if (atoi(token_discrete) < 65536 && atoi (token_discrete) >= 0) {

        for (int i=0;i<65536;i++) {
            seen[i]=0;
            for (int j=0;j<(size_of_file/2);j++) {
                if (buffer[j] == i) {
                    seen[i]++;
                }
            }
            if (atoi (token_discrete) == i) {
                if(!args_info.output_given){
                    printf("byte %05d:%d\n", atoi (token_discrete), seen[i]);
                }else{
                    fprintf(out,"byte %05d:%d\n", atoi (token_discrete), seen[i]);
                }


            }
    
        }                
                        
    }
    }
    if(!args_info.output_given){
        printf("------------------\n");
    }else{
        fprintf(out,"------------------\n"); 
    }

    fclose(fptr);
    token = strtok(NULL, ",");
    }
}

void mode_2(char *token , FILE* out, struct gengetopt_args_info args_info){
    //this would do the same as mode_1 but with 2bytes
    //same logic as mode_2_compact

	//100% funcional (-m 1)
    long int sum=0;
    int notMultiple = 0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
				fptr = fopen(token,"rb"); //open file
            //fptr = fgets()
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

				uint16_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				int seen[65536] = {0};

                if(!args_info.output_given){
                    printf("freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }
                if(args_info.output_given){
                    fprintf(out,"freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }

                if( (size_of_file % 2) != 0){
                    //printf("The size of '%s' is not multiple of '2'!",token);
                    notMultiple = size_of_file % 2;
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        while ( (bytes_read = fread(buffer, sizeof(uint16_t), CHUNK_LEN, fptr)) > 0) {
                            for (size_t i = 0; i < bytes_read; i ++) {
                               // if (buffer[i] > -1 && buffer[i] < 65536) {
                                    seen[buffer[i]]++;
                               // }
                            }
                        }


                        for (int j = 0; j < (65536-notMultiple); j++) {
                                if (seen[j] > 0) {
                                    if(!args_info.output_given){
                                        printf("bi-byte %5d:%d\n", j, seen[j]);
                                    }
                                    if(args_info.output_given){
                                        fprintf(out,"bi-byte %5d:%d\n", j, seen[j]);
                                    }

						            sum = sum + seen[j];
                                 }           
                            
                        }
            

				

                if(!args_info.output_given){
                    printf("sum:%ld\n", sum);
                }
                if(args_info.output_given){
                    fprintf(out,"sum:%ld\n", sum);
                }

				sum = 0; //empty the sum variable for the next file



				fclose(fptr); //closing the file
                if(!args_info.dir_given){
                    token = strtok(NULL, ",");
                }


                if(args_info.dir_given){
                    break;
                }
				
			}
	
}


//mode_4

/* void mode_4_compact(char *token , FILE* out, struct gengetopt_args_info args_info){

	//100% funcional (-m 1)
    long int sum=0;
    int notMultiple = 0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
                int testingRead = 1;
				fptr = fopen(token,"rb"); //open file
            //fptr = fgets()
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

				uint16_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				int seen[65536] = {0};

                if(!args_info.output_given){
				    printf("%s:%ldbytes:", token, size_of_file);
                }
                if(args_info.output_given){
				    fprintf(out,"%s:%ldbytes:", token, size_of_file);
                }

                if( (size_of_file % 2) != 0){
                    //printf("The size of '%s' is not multiple of '2'!",token);
                    notMultiple = size_of_file % 2;
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        while (bytes_read = fread(buffer, sizeof(uint16_t), CHUNK_LEN, fptr)) {
                            size_t found_index;
                            for (size_t i = 0; i < bytes_read; i ++) {
                                if (buffer[i] > -1 && buffer[i] < 65536) {
                                    seen[buffer[i]]++;
                                }
                            }
                        }


                        for (int j = 0; j < (65536-notMultiple); j++) {
                                if (seen[j] > 0) {
                                    if(args_info.output_given){
                                        fprintf(out,"%d", seen[j]);
                                    }
                                    if(!args_info.output_given){
                                        printf("%d", seen[j]);
                                    }

						            sum = sum + seen[j];
                                 }           
                            
                        }
            

				

                    if(!args_info.output_given){
                        printf(":%ld\n", sum);
                    }
                    if(args_info.output_given){
                        fprintf(out,":%ld\n", sum);
                    }

				sum = 0; //empty the sum variable for the next file



				fclose(fptr); //closing the file
                if(!args_info.dir_given){
                    token = strtok(NULL, ",");
                }


                if(args_info.dir_given){
                    break;
                }
				
			}
	
} */

/* 
void mode_4_discrete(char * token, FILE* out, struct gengetopt_args_info args_info,
                    char * token_discrete, char *string_discrete, char *string_of_files) { //COLOCAR SITE STRTOK_R

    char *rest = NULL;
    char *rest_1 = NULL;

    for ((token = strtok_r(string_of_files,",",&rest)); token != NULL; token = strtok_r(NULL,",",&rest)) {
        //printf("%s\n", token);
        FILE * fptr = NULL;
        int testingRead = 1;
        fptr = fopen(token, "rb");

        if (fptr == NULL) {
          	ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
        }

        fseek(fptr, 0L, SEEK_END); //offset made from the end of the file
        long int size_of_file = ftell(fptr);//calculating the size of the file
        size_t bytes_read = 0;

        uint8_t buffer[CHUNK_LEN]; //bufer with the size of file

        fseek(fptr, 0L, SEEK_SET); //go to the beginning of the file
        
        printf("freqCounter:'%s':%ld bytes\n", token, size_of_file);

        for ((token_discrete = strtok_r(string_discrete,",",&rest_1)); token_discrete != NULL;
                                                         token_discrete = strtok_r(NULL,",",&rest_1)) {
            //printf("%s\n", token_discrete);
            //int seen[] = {0};
            int parameter_discrete;
            parameter_discrete = atoi(token_discrete);
            
            if (parameter_discrete <  && parameter_discrete >= 0) {
                        while (bytes_read = fread(buffer, sizeof(uint32_t), CHUNK_LEN, fptr)) {
                            size_t found_index;
                            for (size_t i = 0; i < bytes_read; i ++) {
                                if (buffer[i] > -1 && buffer[i] < ) {
                                    seen[buffer[i]]++;
                                }
                            }
                        }


                        for (int j = 0; j < 65536; j++) {
                                if (parameter_discrete == j) {
                                    if(args_info.output_given){
                                        fprintf(out,"%d", seen[j]);
                                    }
                                    if(!args_info.output_given){
                                        printf("%d", seen[j]);
                                    }
                                 }           
                            
                        }


            }

        } 
        printf("------------------\n");
        fclose(fptr);
    }    
}

void mode_4(char *token , FILE* out, struct gengetopt_args_info args_info){

	//100% funcional (-m 1)
    long int sum=0;
    int notMultiple = 0;

	        while (token != NULL) {      //loop through the string to extract all other tokens
			    FILE *fptr = NULL; 
                int testingRead = 1;
				fptr = fopen(token,"rb"); //open file
            //fptr = fgets()
				if (fptr == NULL) {
				    ERROR(2,"'%s': CANNOT PROCESS FILE\n",token);
				}
				fseek(fptr,0L,SEEK_END); //offset made from the end of the file
				long int size_of_file = ftell(fptr); //calculating the size of the file
                size_t bytes_read = 0;

				uint16_t buffer[CHUNK_LEN]; //buffer
				fseek(fptr,0L,SEEK_SET); //go to the beginning of the file

				//int seen[4294967296] = {0};

                if(!args_info.output_given){
                    printf("freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }
                if(args_info.output_given){
                    fprintf(out,"freqCounter:'%s':%ld bytes\n", token, size_of_file);
                }

                if( (size_of_file % 4) != 0){
                    //printf("The size of '%s' is not multiple of '4'!",token);
                    notMultiple = size_of_file % 4;
                }


    //https://stackoverflow.com/questions/28197649/reading-binary-file-in-c-in-chunks/28197753
    //https://stackoverflow.com/questions/53437381/c-how-to-read-portion-of-a-file-in-chunks
    //We checked those websites to understand better how to read a file in chunks
    //and not the entire file. This to avoid stack overflow if we read >1GB file

                        while (bytes_read = fread(buffer, sizeof(uint16_t), CHUNK_LEN, fptr)) {
                            size_t found_index;
                            for (size_t i = 0; i < bytes_read; i ++) {
                                if (buffer[i] > -1 && buffer[i] < 4294967296) {
                                    seen[buffer[i]]++;
                                }
                            }
                        }


                        for (int j = 0; j < (4294967296-notMultiple); j++) {
                                if (seen[j] > 0) {
                                    if(!args_info.output_given){
                                        printf("quad-byte %10d:%d\n", j, seen[j]);
                                    }
                                    if(args_info.output_given){
                                        fprintf(out,"quad-byte %7d:%d\n", j, seen[j]);
                                    }

						            sum = sum + seen[j];
                                 }           
                            
                        }
            

				

                if(!args_info.output_given){
                    printf("sum:%ld\n", sum);
                }
                if(args_info.output_given){
                    fprintf(out,"sum:%ld\n", sum);
                }

				sum = 0; //empty the sum variable for the next file



				fclose(fptr); //closing the file
                if(!args_info.dir_given){
                    token = strtok(NULL, ",");
                }


                if(args_info.dir_given){
                    break;
                }
				
			}
	
}
*/


// We checked these websites to create all the functions related with -d option
// https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
// https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
//https://stackoverflow.com/questions/1271064/how-do-i-loop-through-all-files-in-a-folder-using-c


void dir(char * string_dir, FILE * out, struct gengetopt_args_info args_info) {

    
  char * output_string; // = NULL;
  struct dirent * dirEntry; // Pointer for directory entry
  // this struct is used to get information from directory entry
  //it is declared in dirent.h

  char * dirVerifier = strstr(string_dir, ",");

  if (dirVerifier != NULL) {
    ERROR(4, "More than 1 directory was inserted!");
  }
  else {
    // This will be used to check if the directory exists
    DIR * dir = opendir(string_dir); //this will try to open the choosen string
    if(dir == NULL){ //this will try to enter the directory, if it is a folder or an invalid directory opendir
        ERROR(8, "cannot access directory '%s'",string_dir);//will return NULL, and errno will get the value
    }                                                          //related with the error
         
    if (dir) {
      // Directory exists
      while ((dirEntry = readdir(dir)) != NULL) { //this will return a pointer to the dirent structure
                                            //It returns NULL on reaching the end of the directory stream
         
         //This if..else will make sure that the files inside that dir are regular files
         //this means not subdirectory/folders and thos files started with '.', this includes
         //'.' and '..'
       if ((dirEntry->d_type == DT_REG) && (dirEntry->d_name[0] != '.' )) {
           //DT_REG is a constant that saves a value for file types (in this case it has the value for regular files)
           //d_type has the type of each file
           //d_name[0] has the first char of each file's name
          output_string = concat_dir(string_dir, dirEntry->d_name);
          //this fuunction returns a file path
          //I will concatenate a directory + '/' + file name and return a complete path to the file

        
             //now this will check mode_arg
             //if there is no mode, that means '-m 1'
          if (!args_info.mode_given) {
            args_info.mode_arg = 1;
            dir_mode(out, args_info, output_string);
            //since !args_info.mode_given is equal to args_mode.mode_arg=1
          }
          else {
            dir_mode(out, args_info, output_string);
          }
          // printf("TEST BEFORE SWITCH");
          ////free(output_string);
          //output_string = NULL;

           // printf("\nDEBUG\nDIR: %s\n\n END OF DEBUG\n",dirEntry->d_name); //The loop is okay!
            

       }
       else{
           ERROR(8, "cannot access directory '%s'",string_dir);
       }


      }

      closedir(dir); //this closes the directory

    }
    else if (ENOENT == errno) {
      //This will check if the error is equal to ENOENT that means "No such file or directory"
      //debug code
      //printf("TESTTTTTTTTT"\n);
      //end of debug code
      ERROR(5, "Directory does not exist.");
    }
    else {
      //If any different error occurs
      //This will happen if the error isn't ENOENT
      ERROR(6, "Failed for some other reason.");
    }

  }
}


char* concat_dir(const char* dir, const char* file_name){
    char* full_path; //pointer to save string and then return it
    asprintf(&full_path, "%s/%s", dir, file_name); // dir + '/' + file_name
    //asprintf() calculates the length of the string,
    // allocates that amount of memory, and writes the string into it.
    //I could have done this in a lot of different ways and used a lot of different functions
    //I could have used sprintf() or vsprintf(), but they need to allocate a buffer first
    //and this one does that for me (= simpler code)

    //https://stackoverflow.com/questions/308695/how-do-i-concatenate-const-literal-strings-in-c
    //checked this website, and got an idea from it
    //so it almost the same thing
    return full_path;
}

//this function will send the complete path to all functions depending on the arguments that were assigned/attributed
void dir_mode(FILE *out,struct gengetopt_args_info args_info, char* string_out){
    if(args_info.mode_arg == 1){
        if (args_info.compact_given)
        {
            mode_1_compact(string_out,out,args_info);
        }
        else if(args_info.discrete_given){
           // mode_1_dicrete(string_out,out,args_info);         
        }
        else{ // mode_1 without discrete or compact
            mode_1(string_out,out,args_info);
        }          
    }

    if(args_info.mode_arg == 2){
        if (args_info.compact_given)
        {
            mode_2_compact(string_out,out,args_info);
        }
        else if(args_info.discrete_given){
           // mode_2_dicrete(string_out,out,args_info);         
        }
        else{ // mode_1 without discrete or compact
            mode_2(string_out,out,args_info);
        }          
    }
    /* 
    if(args_info.mode_arg == 4){
        if (args_info.compact_given)
        {
            mode_4_compact(string_out,out,args_info);
        }
        else if(args_info.discrete_given){
           // mode_4_dicrete(string_out,out,args_info);         
        }
        else{ // mode_1 without discrete or compact
            mode_4(string_out,out,args_info);
        }          
    }
    */
}