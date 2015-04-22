#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include "io.h"
#include "perf.h"
#include "util.h"

#define DEBUG true
/* 
 * This macro requires c99.
 */
#define debug_printf(fmt, ...) \
                            do { if (DEBUG) printf(fmt,## __VA_ARGS__); } while (0)

int main(int argc, const char *argv[])
{
        struct timeval tvStart, tvEnd;
        int err;

        err = gettimeofday(&tvStart, NULL);
        if(err != 0)
                exit(EXIT_FAILURE);

        if(argc == 1) {
                usage(EARGS);
                return(EXIT_FAILURE);
        }

        // Default value for maxthreads
        int maxthreads = 24;
        int files = 0;
        for(int i = 1; i < argc; i++) {
                if(strcmp(argv[i], "-maxthreads") == 0) {
                        if(i > argc - 2) {
                                usage(ETHREADS);
                                return(EXIT_FAILURE);
                        }

                        int tmp = atoi(argv[i+1]);
                        if(tmp <= 0) {
                                usage(ETHREADS);
                                return(EXIT_FAILURE);
                        }

                        maxthreads = tmp;
                        i++;
                        debug_printf("Setting maxthreads to %d.\n", maxthreads);
                }
                else if(strcmp(argv[i], "-stdin") == 0) {
                        debug_printf("Reading flow from stdin.\n");
                }
                else {
                        files++;
                        debug_printf("File detected : %s ", argv[i]);
			if(is_url(argv[i])) {
				debug_printf("(URL).\n");
                                // Lancement d'un thread avec extract_url
                        }
			else {
				debug_printf("(local file).\n");
                                // Lancement d'un thread avec extract_file
			}
                }
        }

        if(files == 0) {
                usage(ENOFILE);
                return(EXIT_FAILURE);
        }

	// Lancement des 'maxthreads' threads de calculs
	
	// Lancement de l'unique thread de sauvegarde (déplacement
	// du second buffer vers la liste chaînée
	
	// Le thread principal lance find_unique

        err = gettimeofday(&tvEnd, NULL);
        if(err != 0)
                return(EXIT_FAILURE);

        // Output
        printf("\n");
        printf("\n");
        printf("%ld\n", timeval_diff(&tvEnd, &tvStart));
}
