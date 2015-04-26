#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <getopt.h>

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

bool file_read = false;
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
	
        int myflag;
	struct option long_options[] = {
		{"maxthreads", required_argument, &myflag, 1},
		{"stdin", no_argument, &myflag, 2},
		{NULL, 0, NULL, 0}
	};

	int option_index = 0;
        int maxthreads = 24;
        int option = 0;

        while((option = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
                switch(option) {
                        case 0:
                                if(*(long_options[option_index].flag) == 1) {
                                        if(atoi(optarg) <= 0) {
                                                usage(ETHREADS);
                                                return(EXIT_FAILURE);
                                        }
                                        
                                        debug_printf("Setting maxthreads option to %s.\n", optarg);
                                        maxthreads = atoi(optarg);
                                } 
                                else if(*(long_options[option_index].flag) == 2) {
                                        debug_printf("Reading flow from stdin.\n");
                                }
                                break;
                        default:
                                exit(EXIT_FAILURE);
                } 
        }
        
	// Semaphores initialization (wrap all this in a function?)
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

        if (optind >= argc) {
                usage(ENOFILE);
                return(EXIT_FAILURE);
        }

        pthread_t extractors[argc-optind];
        for(int i = 0; optind < argc; i++) {
                if(is_url(argv[optind])) {
                        // Lancement d'un thread avec extract_url
                        debug_printf("Creating a thread to read %s (URL).\n", argv[optind]);
                }
                else {
                        // Lancement d'un thread avec extract_file
                        debug_printf("Creating a thread to read %s (local file).\n", argv[optind]);
                        err = pthread_create(&(extractors[i]), NULL, &extract_file,(void *) argv[optind]);
                        if(err != 0)
                                exit(EXIT_FAILURE);
                }

                optind++;
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
        for(int i = 0; i < argc-optind; i++) {
                err = pthread_join(extractors[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);
                
                debug_printf("Extractor has terminated.\n");
        }

	// Si on arrive ici, la lecture des fichier est terminée
	debug_printf("Extraction finished.\n");
        file_read = true;
	
	for(int i = 0; i < maxthreads; i++) {
                // D'office pas bon mais fonctionne avec des petits fichiers pour
                // continuer à avancer en attendant d'avoir trouver une solution.
                // Note : cela provoque un memory leak
                err = pthread_cancel(calculators[i]);//, NULL);
                pthread_join(calculators[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);

                debug_printf("Calculator has terminated.\n");
        }
	debug_printf("Computation finished.\n");

	// Lancement de l'unique thread de sauvegarde (déplacement
	// du second buffer vers la liste chaînée)
	
	// Le thread principal lance find_unique

        err = gettimeofday(&tvEnd, NULL);
        if(err != 0)
                return(EXIT_FAILURE);

        // Output
	printf("result\n");
        printf("filename\n");
        printf("%ldus\n", timeval_diff(&tvEnd, &tvStart));

        // Destroy all semaphores (wrap this in a function?)
        err = sem_destroy(&empty1);
        if(err != 0)
                return(EXIT_FAILURE);

        err = sem_destroy(&empty2);
        if(err != 0)
                return(EXIT_FAILURE);

        err = sem_destroy(&full1);
        if(err != 0)
                return(EXIT_FAILURE);

        err = sem_destroy(&full2);
        if(err !=0)
                return(EXIT_FAILURE);
}
