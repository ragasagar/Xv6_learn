#include "string.h"

void split(char * input, char *deklimiter, char *output){

    // Returns first token
    char *token = strtok(input, deklimiter);
    int i = 0;
    char *result[100];
    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {   

        result[i++]= token;
        token = strtok(NULL, "-");
    }
    output = result;
    return 0;
}