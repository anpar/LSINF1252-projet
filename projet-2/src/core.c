#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <math.h>

#include <curl/curl.h>    
#include "fopen.h"

#include "core.h"
#include "stack.h"
#include "trial.h"

#define DEBUG true
/* 
 * This macro requires c99.
 */
#define debug_printf(fmt, ...) \
                            do { if (DEBUG) printf(fmt,## __VA_ARGS__); } while (0)

// Liste chaînée contenant les facteurs premiers
struct node * list;

// Premier buffer
pthread_mutex_t mutex1;
struct node * buffer1;   
extern sem_t empty1;
extern sem_t full1;

// Second buffer
pthread_mutex_t mutex2;
struct node * buffer2;
extern sem_t empty2;
extern sem_t full2;

// Variables contenant les conditions d'arrêt des threads
extern bool file_read;
extern bool fact_done;

extern volatile int active_readers;
extern volatile int active_factorizers;

bool is_empty_buffer1 = false;
bool is_empty_buffer2 = false;

extern pthread_mutex_t rd;

void * extract_file(void * filename) 
{
        pthread_mutex_lock(&active_readers_mutex);
        active_readers++;
        pthread_mutex_unlock(&active_readers_mutex);
	URL_FILE *handle;
	int err;
        /*CURL *curl = curl_easy_init();
        if(curl == NULL) {
                fprintf(stderr, "Error while initializing libcurl.\n");
                exit(EXIT_FAILURE);
        }

        CURLcode curlcode = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, true);*/

        char *file = (char *) filename;
        
        //debug_printf("Trying to open %s...", file);
        handle = url_fopen(file, "rb");
        // Ne détecte pas si le fichier n'existe pas...
        if(handle == NULL) {
		fprintf(stderr, "Error while opening %s.\n", file);
                exit(EXIT_FAILURE);	
	}
        //debug_printf(" success!\n");
	
        // Initialize new to 0 and NULL by default
	struct number new = {0, NULL};
	uint64_t n;
        bool empty = true;
	while(url_fread(&n, sizeof(uint64_t), 1, handle) != 0) {
		empty = false;
                //debug_printf("Reading %s : ", file);
		n = be64toh(n);
                //debug_printf("%" PRIu64 "\n", n);
		(&new)->n = n;
		(&new)->origin = file;
		sem_wait(&empty1);
	        pthread_mutex_lock(&mutex1);
		push(&buffer1, new);
		pthread_mutex_unlock(&mutex1);
		sem_post(&full1);
        }
        
        //debug_printf("Trying to close %s...", file);
	err = url_fclose(handle);
	if(err != 0) {
		fprintf(stderr, "Error while closing %s.\n", file);
	}
        //debug_printf(" success!\n");
        
        //curl_easy_cleanup(curl);
        //debug_printf("Leaving extract_file.\n");

        pthread_mutex_lock(&active_readers_mutex);
        active_readers--;
        pthread_mutex_unlock(&active_readers_mutex);

        // Rustine pour détecter les fichiers vides/non-existants
        // Ne fonnctionne pas pour les URL, comportements indéfinis.
        if(empty) {
                fprintf(stderr, "%s is either empty or non-existing.\n", file);
                exit(EXIT_FAILURE);
        }

        pthread_exit(NULL);
}

void prime_factorizer(uint64_t n, char * origin)
{
        uint64_t r = trial_division(n);
        struct number new = {0, NULL};
        if(r == IS_PRIME) {
                (&new)->n = n;
                //debug_printf("Factor : %" PRIu64 "\n", n);
                (&new)->origin = origin;
                sem_wait(&empty2); 
                pthread_mutex_lock(&mutex2);
                push(&buffer2, new);
                pthread_mutex_unlock(&mutex2);
                sem_post(&full2);
        } else {
                prime_factorizer(r, origin);
                prime_factorizer(n/r, origin);
        }
}

void * factorize(void * n)
{
        pthread_mutex_lock(&active_factorizers_mutex);
        active_factorizers++;
        pthread_mutex_unlock(&active_factorizers_mutex);
        //debug_printf("In factorize.\n");
        int err;
	struct number *item = (struct number *) malloc(sizeof(struct number));
        while(!(file_read && is_empty_buffer1)) {
                errno = 0;
                err = sem_trywait(&full1); 
                // Attente d'un slot rempli, s'il il n'y en
                // a pas, on passe le bloc suivant et on
                // exécute encore la boucle. S'il y en a un
                // on rentre dans le bloc suivant.
                if(!(err != 0 && errno == EAGAIN)) {
		        pthread_mutex_lock(&mutex1);
                        is_empty_buffer1 = pop(&buffer1, item);
                        //printf("Buffer 1 (factorize) : "); display(buffer1);
                        pthread_mutex_unlock(&mutex1);
                        sem_post(&empty1);
                        
                        //if(!is_empty_buffer1)
                        prime_factorizer(item->n, item->origin);
                        // FIX : I move the two following lines above because prime_factorize
                        // doesn't need to between those mutes/sem. Note : seems legit to me
                        // but causes annoying bug in some cases... Let's create an issue on GitHu
                        //pthread_mutex_unlock(&mutex1);
		        //sem_post(&empty1); // Il y a un slot libre en plus              
                
                }
		else {
                        pthread_mutex_lock(&mutex1);
                        is_empty_buffer1 = is_empty(buffer1);
                        pthread_mutex_unlock(&mutex1);
                }
        }

        pthread_mutex_lock(&active_factorizers_mutex);
        active_factorizers--;
        pthread_mutex_unlock(&active_factorizers_mutex);
        free(item);
	pthread_exit(NULL);
}

void insert(struct number * new_number)
{
        //debug_printf("In insert\n");
        struct node * new = (struct node *) malloc(sizeof(struct node));
        if(new == NULL)
                exit(EXIT_FAILURE);

        new->content = *new_number;
        new->next = NULL;
        struct node * current;
        if(list == NULL || list->content.n > new->content.n) {
                new->next = list;
                list = new;
        }
        else if(list->content.n == new->content.n) {
                list->content.origin = NULL;
                free(new);
        }
        else {
                current = list;
                while(current->next != NULL && current->next->content.n < new->content.n) {
                        current = current->next;
                }
                
                if(current->next == NULL) {
                        current->next = new;
                }
                else if(current->next->content.n == new->content.n) {
                        current->next->content.origin = NULL;
                        free(new);
                }
                else {
                        new->next = current->next;
                        current->next = new;
                }
        }
}

void print_list()
{
        struct node * current = list;
        while(current != NULL) {
                printf("%" PRIu64 "(%s) - ", current->content.n, current->content.origin);
                current = current->next;
        }
        printf("\n");
}

void * save_data(void * n) 
{
        //debug_printf("In save_data\n");
        int err;
	struct number *item = (struct number *) malloc(sizeof(struct number));
        while(!(fact_done && is_empty_buffer2)) {
                errno = 0;
                err = sem_trywait(&full2); 
                // Attente d'un slot rempli, s'il il n'y en
                // a pas, on passe le bloc suivant et on
                // exécute encore la boucle. S'il y en a un
                // on rentre dans le bloc suivant.
                if(!(err != 0 && errno == EAGAIN)) {
		        pthread_mutex_lock(&mutex2);
                        is_empty_buffer2 = pop(&buffer2, item);
                        //printf("Buffer 2 (save_data) :"); display(buffer2);
                        if(!is_empty_buffer2)
                                insert(item);
                        //printf("List : "); print_list();
                        pthread_mutex_unlock(&mutex2);
		        sem_post(&empty2); // Il y a un slot libre en plus              
                }
                else {
                        pthread_mutex_lock(&mutex2);
                        is_empty_buffer2 = is_empty(buffer2);
                        pthread_mutex_unlock(&mutex2);
                }
        }

        free(item);
	pthread_exit(NULL); 
}

void free_list()
{
        struct node * tmp1;
        struct number * tmp2;

	while(list != NULL) {
		tmp1 = list;
                tmp2 = &(list->content);
                list = list->next;
                free(tmp1);
                free(tmp2);
	}
}

int find_unique(struct number * unique)
{
        if(list == NULL)
                return(EXIT_FAILURE);

	//Create a node in order to check the list node by node
	struct node *runner;
	runner = list;

        struct node *tmp;

	while(runner->next != NULL && runner->content.origin == NULL) {
		tmp = runner;
                runner = runner->next;
                free(tmp);
	}

	if(runner->content.origin != NULL) { 
		*unique = runner->content;
                while(runner != NULL) {
                        tmp = runner;
                        runner = runner->next;
                        free(tmp);
                }
	        return(EXIT_SUCCESS);
        }

        free(runner);
	return(EXIT_FAILURE);
}
