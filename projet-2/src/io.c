#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

void usage(int error)
{
        if(error == EARGS) {
                fprintf(stderr, "./upfact: invalid number of arguments\n");
                fprintf(stderr, "Example:\n");
                fprintf(stderr, "./upfact -maxthreads 8 file1 file2 https://example.org/file\n");
                fprintf(stderr, "Note that -maxthreads also set the size of the first buffer.\n");
                fprintf(stderr, "You can also use -stdin to read flow from terminal.\n");
        }
        else if(error == ETHREADS) {
                fprintf(stderr, "./upfact: maxthreads must be a positive integer\n");
        }
        else if(error == ENOFILE) {
                fprintf(stderr, "./upfact: at least one file needed\n");
        }
}
