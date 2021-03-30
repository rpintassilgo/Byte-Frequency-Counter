// Rodrigo Pintassilgo - 2191190
//Daniel Patricio - 2191195





#ifndef MODE_COMPACT_H_INCLUDED
#define MODE_COMPACT_H_INCLUDED

//mode_1
void mode_1_compact(char *token , FILE* out, struct gengetopt_args_info args_info);
void mode_1_discrete(char * token, FILE* out, struct gengetopt_args_info args_info,
                    char * token_discrete, char *string_discrete);
void mode_1(char *token , FILE* out, struct gengetopt_args_info args_info);


//mode_2
void mode_2_compact(char *token, FILE* out, struct gengetopt_args_info args_info);
void mode_2_discrete(char * token, FILE* out, struct gengetopt_args_info args_info, 
                                    char * token_discrete,char *string_discrete);
void mode_2(char *token, FILE* out, struct gengetopt_args_info args_info);


/* mode_4
void mode_4_compact(char *token);
void mode_4_discrete(char *token); */
//void mode_4(char *token , FILE* out, struct gengetopt_args_info args_info);


//dir 
void dir(char *string_dir,FILE* out, struct gengetopt_args_info args_info);
char* concat_dir(const char* dir, const char* file_name);
void dir_mode(FILE *out,struct gengetopt_args_info args_info, char* string_out);

#endif // MODE_COMPACT_H_INCLUDED
