#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <getopt.h>

#include <curl/curl.h>
#include "fopen.h"

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

// Semaphores for the two buffers
sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;
// Linked list of prime factors
struct node * list;

bool file_read = false;
bool fact_done = false;

volatile int active_readers = 0;
volatile int active_factorizers = 0;

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
        int maxthreads = 4;
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
                                        
                                        maxthreads = atoi(optarg);
                                } 
                                else if(*(long_options[option_index].flag) == 2) {
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
        
        // Si aucun fichier n'est donné en argument et qu'on ne lit pas non plus depuis stdin
        if (optind >= argc && !read_from_stdin) {
                usage(ENOFILE);
                return(EXIT_FAILURE);
        }
        
        CURLcode curl = curl_global_init(CURL_GLOBAL_ALL);
        if(curl != 0) {
                fprintf(stderr, "Error while initializing libcurl.\n");
                exit(EXIT_FAILURE);
        }

        // filec contient le nombre de fichiers à lire (hors stdin)
        unsigned int filec = argc-optind;
        pthread_t extractors[filec];
        for(int i = 0; optind < argc; i++) {
                // Lancement d'un thread avec extract_file
                debug_printf("Creating extractor %d to read %s.\n", i, argv[optind]);
                err = pthread_create(&(extractors[i]), NULL, &extract_file,(void *) argv[optind]);
                if(err != 0)
                        exit(EXIT_FAILURE);

                optind++;
        }

        //curl_global_cleanup();
                
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

        int last_active_readers;
        do {
                //debug_printf("Waiting for readers.\n");
                pthread_mutex_lock(&active_readers_mutex);
                last_active_readers = active_readers;
                pthread_mutex_unlock(&active_readers_mutex);
        } while(last_active_readers != 0);

        // Récupération et libération des threads extractors
        // Remarque: on ne rentre dans la boucle que si files != 0.
        for(int i = 0; i < filec; i++) {
                debug_printf("Trying to join extractor %d.\n", i);
                err = pthread_join(extractors[i], NULL);
                if(err != 0)
                        exit(EXIT_FAILURE);
                
                debug_printf("Extractor has terminated.\n");
        }

	// Si on arrive ici, la lecture des fichier est terminée
	debug_printf("Extraction finished.\n");
        file_read = true;
	
        int last_active_factorizers;
        do {
                pthread_mutex_lock(&active_factorizers_mutex);
                last_active_factorizers = active_factorizers;
                pthread_mutex_unlock(&active_factorizers_mutex);
        } while(last_active_factorizers != 0);

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
        printf("%.3lfs\n", timeval_diff(&tvEnd, &tvStart));

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
