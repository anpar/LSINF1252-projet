#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
/*
 * Fix, how to install and include this?
 */
//#include <curl/curl.h>

#include "core.h"
#include "stack.h"
#include "squfof.h"

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

extern pthread_mutex_t rd;

void * extract_file(void * filename) {
        //debug_printf("In extract_file.\n");
	FILE *f;
	int err;
        char *file = (char *) filename;
	f = fopen(file, "rb");
        if(f == NULL) {
		fprintf(stderr, "Error while opening %s.\n", file);
		exit(EXIT_FAILURE);	
	}
	
        // Initialize new to 0 and NULL by default
	struct number new = {0, NULL};
	uint64_t n;
	while(fread(&n, sizeof(uint64_t), 1, f) != 0) {
	//while(!feof(f)) {
               // fread(&n, sizeof(uint64_t), 1, f);
		debug_printf("In the loop of extract_file.\n");
		debug_printf("1:%" PRIu64 "\n", n);
		n = be64toh(n);
		debug_printf("2:%" PRIu64 "\n", n);
		(&new)->n = n;
		(&new)->origin = file;
		sem_wait(&empty1); // Attente d'un slot libre
	        pthread_mutex_lock(&mutex1);
		push(&buffer1, new);
                //printf("Buffer 1 (extract_file) : "); display(buffer1);
		pthread_mutex_unlock(&mutex1);
		sem_post(&full1); // Il y a un slot rempli en plus
		//fscanf(f, "%u", &n);
        }

	err = fclose(f);
	if(err != 0) {
		fprintf(stderr, "Error while closing %s.\n", file);
	}
       
        debug_printf("Leaving extract_file.\n");
        pthread_exit(NULL);
}

void prime_factorizer(unsigned int n, char * origin)
{
        //debug_printf("In prime_factorizer.\n");
        unsigned int r = SQUFOF(n);
        struct number new = {0, NULL};
        if(r == n) {
                (&new)->n = r;
                (&new)->origin = origin;
                sem_wait(&empty2); // Attendre d'un slot de libre
                pthread_mutex_lock(&mutex2);
                push(&buffer2, new);
                //printf("Buffer 2 (prime_factorizer) : "); display(buffer2);
                pthread_mutex_unlock(&mutex2);
                sem_post(&full2); // Il y a un slot rempli de plus
        }
        else {
                prime_factorizer(r, origin);
                prime_factorizer(n/r, origin);
        }
}

void * factorize(void * n)
{
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
                        //pthread_mutex_unlock(&mutex1);
                        //sem_post(&empty1);
                        
                        if(!is_empty_buffer1)
                                prime_factorizer(item->n, item->origin);
                        // FIX : I move the two following lines above because prime_factorize
                        // doesn't need to between those mutes/sem. Note : seems legit to me
                        // but causes annoying bug in some cases... Let's create an issue on GitHu
                        pthread_mutex_unlock(&mutex1);
		        sem_post(&empty1); // Il y a un slot libre en plus              
                
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
                printf("%d (%s) - ", current->content.n, current->content.origin);
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
