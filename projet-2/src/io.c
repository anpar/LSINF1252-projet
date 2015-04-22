#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

void usage(int error)
{
        if(error == EARGS) {
                fprintf(stderr, "Invalid number of arguments.\n");
                fprintf(stderr, "Example:\n");
                fprintf(stderr, "./upfact -maxthreads 8 file1 file2 https://example.org/file\n");
        }
        else if(error == ETHREADS) {
                fprintf(stderr, "Invalid argument : maxthreads must be a positive integer.\n");
        }
        else if(error == ENOFILE) {
                fprintf(stderr, "At least one file needed.\n");
        }
}
