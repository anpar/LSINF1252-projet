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

#include <curl/curl.h>    
#include "fopen.h"

#include "core.h"
#include "stack.h"
#include "trial.h"

#define DEBUG false
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

bool is_empty_buffer1 = false;
bool is_empty_buffer2 = false;

void * extract_file(void * filename) 
{
	URL_FILE *handle;
	int err;

        char *file = (char *) filename;
        
        handle = url_fopen(file, "rb");
        // Ne détecte pas si le fichier n'existe pas...
	// Sands doute une erreur de libcurl.
        if(handle == NULL) {
		fprintf(stderr, "Error while opening %s.\n", file);
                exit(EXIT_FAILURE);	
	}
	
        // Initialize new to 0 and NULL by default
	struct number new = {0, NULL};
	uint64_t n;
        bool empty = true;
	while(url_fread(&n, sizeof(uint64_t), 1, handle) != 0) {
		empty = false;
		n = be64toh(n);
		(&new)->n = n;
		(&new)->origin = file;
		sem_wait(&empty1);
	        pthread_mutex_lock(&mutex1);
		push(&buffer1, new);
		pthread_mutex_unlock(&mutex1);
		sem_post(&full1);
        }
        
	err = url_fclose(handle);
	if(err != 0) {
		fprintf(stderr, "Error while closing %s.\n", file);
	}
        
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
        int err;
	struct number *item = (struct number *) malloc(sizeof(struct number));
	if(item == NULL)
		exit(EXIT_FAILURE);

        while(!(file_read && is_empty_buffer1)) {
                errno = 0;
                err = sem_trywait(&full1); 
                // Attente d'un slot rempli, s'il il n'y en
                // a pas, on passe le bloc de code suivant et on
                // exécute encore la boucle. S'il y en a un
                // on rentre dans le bloc suivant.
                if(!(err != 0 && errno == EAGAIN)) {
		        pthread_mutex_lock(&mutex1);
                        is_empty_buffer1 = pop(&buffer1, item);
                        pthread_mutex_unlock(&mutex1);
                        sem_post(&empty1);
                        
                        prime_factorizer(item->n, item->origin);
                }
		else {
                        pthread_mutex_lock(&mutex1);
                        is_empty_buffer1 = is_empty(buffer1);
                        pthread_mutex_unlock(&mutex1);
                }
        }

        free(item);
	pthread_exit(NULL);
}

void insert(struct number * new_number)
{
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

// Utiliser pour débugger le programme.
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
        int err;
	struct number *item = (struct number *) malloc(sizeof(struct number));
	if(item == NULL)
		exit(EXIT_FAILURE);

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
                        if(!is_empty_buffer2)
                                insert(item);
                        pthread_mutex_unlock(&mutex2);
		        sem_post(&empty2);              
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
