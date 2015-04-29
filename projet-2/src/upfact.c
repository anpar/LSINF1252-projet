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
struct node * list;

bool file_read = false;
bool fact_done = false;

int main(int argc, char *argv[])
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
        bool read_from_stdin = false;

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
                                        // Lancement d'un thread avec extract_file
                                        debug_printf("Creating a thread to read from sdtin.\n");
					pthread_t stdin_reader;                                        
					err = pthread_create(&stdin_reader, NULL, &extract_file,(void *) "/dev/stdin");
					if(err != 0)
						exit(EXIT_FAILURE);

                                        read_from_stdin = true;
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

        if (optind >= argc && !read_from_stdin) {
                usage(ENOFILE);
                return(EXIT_FAILURE);
        }
        
        // Number of files
        unsigned int filec = argc-optind;
        pthread_t extractors[filec];
        for(int i = 0; optind < argc; i++) {
                // FIX : I heard that we can avoid checking if it's url because libcurl
                // allows us to read URL exactly like local file.
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
	
	// Lancement de l'unique thread de sauvegarde (déplacement
	// du second buffer vers la liste chaînée)
        pthread_t saver;
        debug_printf("Starting data saver.\n");
	err = pthread_create(&saver, NULL, &save_data, NULL); 
        if(err != 0)
                exit(EXIT_FAILURE);

        // Récupération et libération des threads extractors
        // Remarque: on ne rentre dans la boucle que si files != 0.
        for(int i = 0; i < filec; i++) {
                err = pthread_join(extractors[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);
                
                debug_printf("Extractor has terminated.\n");
        }

	// Si on arrive ici, la lecture des fichier est terminée
	debug_printf("Extraction finished.\n");
        file_read = true;
	
	for(int i = 0; i < maxthreads; i++) {
                err = pthread_join(calculators[i], NULL);
                pthread_join(calculators[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);

                debug_printf("Calculator has terminated.\n");
        }
	debug_printf("Computation finished.\n");
        fact_done = true;

        err = pthread_join(saver, NULL);
        if(err != 0)
                exit(EXIT_FAILURE);
        debug_printf("Data saver has terminated.\n");

	// Le thread principal lance find_unique
        debug_printf("Starting find_unique().\n");
        struct number result; 
        err = find_unique(&result);
        if(err == EXIT_FAILURE) {
                free(list);
                debug_printf("No unique prime factor.\n");
                return(EXIT_FAILURE);
        }
        debug_printf("find_unique() done.\n");

        err = gettimeofday(&tvEnd, NULL);
        if(err != 0)
                return(EXIT_FAILURE);

        // Output
	printf("%d\n", result.n);
        printf("%s\n", result.origin);
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

        return(EXIT_SUCCESS);
}
