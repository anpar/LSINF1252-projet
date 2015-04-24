#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#include "io.h"
#include "perf.h"
#include "util.h"
#include "core.h"

#define DEBUG true
/* 
 * This macro requires c99.
 */
#define debug_printf(fmt, ...) \
                            do { if (DEBUG) printf(fmt,## __VA_ARGS__); } while (0)

sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;

bool file_red = false;
bool fact_done = false;

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
        // Variable to count the number of files
        int files = 0;
        // Array to store filenames (max argc files)
        const char *filenames[argc];
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
                        filenames[files] = argv[i];
                        files++;
                        debug_printf("File detected : %s.\n", argv[i]);
                }
        }
        
	// Semaphores initialization
        err = sem_init(&empty1, 0, maxthreads);
        if(err != 0)
                exit(EXIT_FAILURE);
	
	err = sem_init(&empty2, 0, maxthreads);
	if(err != 0)
		exit(EXIT_FAILURE);	

        err = sem_init(&full1, 0, 0);
        if(err != 0)
                exit(EXIT_FAILURE);

	err = sem_init(&full2, 0, 0);
	if(err != 0)
		exit(EXIT_FAILURE);

        pthread_t extractors[files];
        if(files == 0) {
                usage(ENOFILE);
                return(EXIT_FAILURE);
        }
        else {
               // pthread_t extractors[files];
                for(int i = 0; i < files; i++) {
                       if(is_url(filenames[i])) {
                                // Lancement d'un thread avec extract_url
                                debug_printf("Creating a thread to read %s (URL).\n", filenames[i]);
                       }
                       else {
                                // Lancement d'un thread avec extract_file
                                debug_printf("Creating a thread to read %s (local file).\n", filenames[i]);
                                err = pthread_create(&(extractors[i]), NULL, &extract_file,(void *) filenames[i]);
                                if(err != 0)
                                        exit(EXIT_FAILURE);
                       }
                }
        }
        
	// Lancement des threads de calculs
	pthread_t calculators[maxthreads];
	for(int i = 0; i < maxthreads; i++) {
		err = pthread_create(&(calculators[i]), NULL, &factorize, NULL);
		if(err != 0)
			exit(EXIT_FAILURE);

		debug_printf("Creating calculators...\n");
	}
	
        // Récupération et libération des threads extractors
        // Remarque: on ne rentre dans la boucle que si files != 0.
        for(int i = 0; i < files; i++) {
                err = pthread_join(extractors[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);
                
                debug_printf("Extractor on %s has terminated.\n", filenames[i]);
        }

	// Si on arrive ici, la lecture des fichier est terminée
	debug_printf("Extraction finished.\n");
	file_red = true;			
	
	for(int i = 0; i < maxthreads; i++) {
                err = pthread_join(calculators[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);

                debug_printf("Calculator has terminated.\n");
        }
	debug_printf("Computation finished.\n");

	// Lancement de l'unique thread de sauvegarde (déplacement
	// du second buffer vers la liste chaînée
	
	// Le thread principal lance find_unique

        err = gettimeofday(&tvEnd, NULL);
        if(err != 0)
                return(EXIT_FAILURE);

        // Output
	printf("result\n");
        printf("filename\n");
        printf("%ldus\n", timeval_diff(&tvEnd, &tvStart));
}
