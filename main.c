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

#include <time.h> //This library will be needed for "--time"
#include "debug.h"
#include "memory.h"
#include "cmdline.h"
#include "mode_compact.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif


int main(int argc, char * argv[]) {
  struct gengetopt_args_info args_info; // It declares a structure ggo arguments


  if (cmdline_parser(argc, argv, & args_info) != 0  ) { // It tests which parameters were passed
    exit(1);
  }

  FILE *out; // It declares a pointer of type file that will be used in option --output
  char *string_output; // It declares a string that will save 'output_arg'
  clock_t clock_begin = clock(); // clock() returns the number of clock ticks elapsed since the program was launcher
  char *string_of_files; // It declares a pointer of type char
  char *string_discrete; // It declares a pointer of type char
  char *string_dir; // It declares a pointer of type char
  char *token; // It declares a pointer of type char
  char *token_discrete; // It declares a pointer of type char

    if(args_info.output_given){ // If the output argument is given
      string_output =  malloc(strlen(args_info.output_arg) + 1); // It allocates memory for the string
      strcpy(string_output,args_info.output_arg); // It copies the string of the output to the string_output pointer
      if(string_output == NULL){ 
        ERROR(1, "The required memory for the string wasn't allocated!\n"); // If doesn't allocates memory for the string shows this message
      }

      out = fopen (string_output, "w+"); // It opens the file for writing and reading. Creates the file if it does not exist
    }

  
  if(args_info.file_given){ // If the file argument is given allocates memory for the string
    string_of_files = malloc(strlen(args_info.file_arg) + 1); // sizeof(char) is equal to 1
  }
  

  if(args_info.discrete_given){ // If the discrete argument is given
    string_discrete = malloc(strlen(args_info.discrete_arg) + 1); // Allocates memory for the string
  }

  if(args_info.dir_given){ // If dir is given, we will save it inside string_dir and then we will call our dir function to do the job and return "string_of_files"
   
  string_dir = malloc(strlen(args_info.dir_arg) + 1); // It allocates memory for the string
  strcpy(string_dir,args_info.dir_arg); // It copies the string of the dir argument to the string_dir pointer
  }

  if(args_info.file_given){ // If the file argument is given
    if (string_of_files == NULL) { 
      ERROR(1, "The required memory for the string wasn't allocated!\n"); // If doesn't allocates memory for the string shows this message
    }
  }

  if(args_info.dir_given){ // If the dir argument is given
    if (string_dir == NULL) {
      ERROR(1, "The required memory for the string wasn't allocated!\n"); // If doesn't allocates memory for the string shows this message
    } 
  }
 
  if(args_info.discrete_given){ // If the discrete argument is given
    if (string_discrete == NULL) {
      ERROR(1, "The required memory for the string wasn't allocated!\n"); // If doesn't allocates memory for the string shows this message
    }
  }


  if(args_info.file_given){ // If the file argument is given
    strcpy(string_of_files, args_info.file_arg); // It copies the string of the file argument to the string_of_files pointer 
  }

  if(args_info.discrete_given){ // If the discrete argument is given
    strcpy(string_discrete,args_info.discrete_arg); // It copies the string of the discrete argument to the string_discrete pointer
  }

  if(args_info.file_given){ // If the file argument is given
    token = strtok(string_of_files, ","); // This function divides a string into several parts using a delimiter. 
                                          // It returns a pointer to the first token found in the string. Returns a null pointer if there are no more tokens to retrieve.
    // It divides the string by commas in order to obtain a string from the name of each file.
  }
 


  
  if(args_info.no_help_given){
    printf("\t\t\t\tHELP DOCUMENTATION\n");
    printf("OPTIONS:\n");
    printf("\t|OPTIONAL| --compact/-c: This option makes the output sma\n");
    printf("\t --dir/-d: This option let the user use a directory to load files,\n");
    printf("\t\t and load every regular file from it\n");
    printf("\t|OPTIONAL| --discrete/(not avaiable): THis option let the user search specific bytes\n");
    printf("\t --file/-f:\n");
    printf("\t|OPTIONAL| --no-help/-h: It prints this help menu!\n");
    printf("\t|OPTIONAL| --mode/-m: (OPTIONAL=1) 1-it counts 1B  2-it counts 2B 4-it count 4B\n");
    printf("\t|OPTIONAL| --output/-o: It print everything to a specific file\n");
    printf("\t|OPTIONAL| --search/-s: Search an hex pattern and print its offsets\n");
    printf("\t|OPTIONAL| --time/(not avaiable): Print how long the program takes to run (from start to end)\n");
    printf("\n\tMORE INFORMATION:\n");
    printf("\t It is required to use --dir/-f or --file/-f, the user cannot use both.\n");
    printf("--search/-s and -m 4 are not avaiable!\n ");
    printf("--discrete/-d is not fully functional!\n");
    printf("\nRodrigo Pintassilgo - 2191190\n");
    printf("Daniel Patricio - 2191195\n");

  }
  else if((args_info.file_given || args_info.dir_given) && (!( args_info.file_given && args_info.dir_given)) ) { // If file argument or dir argument is given and both are not given at the same time
    if ((args_info.compact_given && args_info.discrete_given) || (args_info.compact_given && args_info.search_given)) { // The compact argument isn't compatible with discrete and search arguments
      printf("The -c/--compact option isn't compatible with --discrete and -s/--search options\n");
    }
    else if((args_info.mode_given && args_info.search_given) || (args_info.discrete_given && args_info.search_given) || (args_info.compact_given && args_info.search_given)){ // 
      printf("The -s/--search option isn't compatible with -m/--mode, -d/--discrete and -c/--compact\n"); // The search argument isn't compatible with mode, discrete and compact arguments
    }
	  else {
      if(args_info.dir_given) { // If the dir argument is given
        dir(string_dir,out,args_info); // It calls the dir function
      }
      else{
        if (!args_info.mode_given) { // If the mode argument isn't given
        	if (args_info.compact_given) { // If the compact argument is given
          	mode_1_compact(token,out,args_info); // It calls the mode_1_compact function
       		}
          else if (args_info.discrete_given) { // If the discrete argument is given
          	mode_1_discrete(token,out,args_info,token_discrete,string_discrete); // It calls the mode_1_discrete function
          }
      		else { 
        		mode_1(token,out,args_info); // It calls the mode_1 function
      		}
        }
        if (args_info.mode_given) { // If the mode argument is given
          if (args_info.mode_arg == 1) { // If the mode argument is 1
            if (args_info.compact_given) { // If the compact argument is given
              mode_1_compact(token,out,args_info); // It calls the mode_1_compact function
            }
		        else if (args_info.discrete_given) { // If the discrete argument is given
              mode_1_discrete(token,out,args_info,token_discrete,string_discrete); // It calls the mode_1_discrete function
            }
		        else {
              mode_1(token,out,args_info); // It calls the mode_1 function
            }
          }
          else if (args_info.mode_arg == 2) { // If the mode argument is 2
            if (args_info.compact_given) { // If the compact argument is given
              mode_2_compact(token,out,args_info); // It calls the mode_2_compact function
            }
		        else if (args_info.discrete_given) { // If the discrete argument is given
              mode_2_discrete(token,out,args_info,token_discrete,string_discrete); // It calls the mode_2_discrete function
            }
		        else {
              mode_2(token,out,args_info); // It calls the mode_2 function
            }
          }
          else if (args_info.mode_arg == 4) { // If the mode argument is 4
            if (args_info.compact_given) { // If the compact argument is given
              // mode_4_compact(token); // It calls the mode_4_compact function
            }
	    	    else if (args_info.discrete_given) { // If the discrete argument is given
              //mode_4_discrete(token); // It calls the mode_4_discrete function
            }
	    	    else {
              //mode_4(token,out,args_info); // It calls the mode_4 function
            }
          }
          else {
            printf("ERROR: invalid value 'VALOR' for -m/--mode.\n"); // If the mode argument is 
          }                                                 //different than 1, 2 or 4 shows this message
        }
      }
    }
  }
  else if(args_info.file_given && args_info.dir_given) {
    printf("The -f/--file option isn't compatible with -d/--dir option\n"); // If the user gives dir and file at the same time, 
                                                                            //this will print an error
  }
  else{
    printf("ERROR\n"); 
  }


  if(args_info.file_given){ // If the file argument is given
    free(string_of_files); // Deallocates the memory previously allocated by malloc
    string_of_files = NULL; // Sets the pointer to null
  }

  if(args_info.discrete_given){ // If the discrete argument is given
    free(string_discrete); // Deallocates the memory previously allocated by malloc
    string_discrete = NULL; // Sets the pointer to null
  }

  if(args_info.dir_arg){ // If the dir argument is given
    free(string_dir); // Deallocates the memory previously allocated by malloc
    string_dir = NULL; // Sets the pointer to null
  }

  if(args_info.time_given){ // If the time argument is given
    clock_t clock_end = clock();
    double time = (double)(clock_end - clock_begin)/ CLOCKS_PER_SEC;
    //this code will give us the time in seconds since it is being divided with CLOCKS_PER_SEC,
    //that is defined in <time.h> and tell us how many clock ticks is one second.
    printf("time: %.7f seconds\n", time);
  }

  if(args_info.output_given){ // If the output argument is given
    free(string_output); // Deallocates the memory previously allocated by malloc
    string_output = NULL; // Sets the pointer to null
    fclose(out); // Closes the file
  }

cmdline_parser_free(&args_info); // Releases resources allocated to gengetopt 

return 0;
}