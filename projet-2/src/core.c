#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "core.h"
#include "stack.h"
#include "squfof.h"

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
extern bool file_red;
extern bool fact_done;

void * extract_file(void * filename) {
	FILE *f;
	int err;
        char *file = (char *) filename;
	f = fopen(file, "r");
        if(f == NULL) {
		fprintf(stderr, "Error while opening %s.\n", file);
		exit(EXIT_FAILURE);	
	}
	
        // Initialize new to 0 and NULL by default
	struct number new = {0, NULL};
	unsigned int n;
	fscanf(f, "%u", &n);
	while(!feof(f))	{
                //fscanf(f, "%u", &n);
		(&new)->n = n;
		(&new)->origin = file;
		sem_wait(&empty1);
	        pthread_mutex_lock(&mutex1);
		push(&buffer1, &new);
                //display(buffer1);
		pthread_mutex_unlock(&mutex1);
		sem_post(&full1);
		fscanf(f, "%u", &n);
        }

	err = fclose(f);
	if(err != 0) {
		fprintf(stderr, "Error while closing %s.\n", file);
	}
        
        pthread_exit(NULL);
}

void * factorize(void * n)
{
	struct number *item;
	bool cond = true;
	while(cond) 
		// PROBLEME ICI
		if(file_red && is_empty(buffer1)) {
                        debug_printf("In condition.\n");
                        cond = false;
                        fact_done = true;
                }
		else {
			sem_wait(&full1);
			pthread_mutex_lock(&mutex1);
			item = pop(&buffer1);
			printf("item : %d.\n", item->n);
			pthread_mutex_unlock(&mutex1);
			sem_post(&empty1);
		}
	}	

	pthread_exit(NULL);
}


/* In comment since it doesn't work
int insert(struct number new)
{
	struct node *runner;
	runner = list;
	struct node *nodenew;
	nodenew->content = new;	

	// Cas début de la liste
	if(runner->content.n > new.n) { 
		nodenew->next = runner;
		runner->content = new;
	}

	// Boucle dans la liste jusqu'a etre juste avant 
	// l'endroit de l'insertion (ou au bout)
	while(runner->next != NULL && runner->next->content.n < new.n) {
		runner = runner->next;	
	}

	// Cas fin de la liste
	if(runner->next == NULL) { 
		nodenew->next = NULL;
		runner->next = nodenew;
	}
	// Cas deja present dans la liste, l'origine est passée a null
	else if(runner->next->content.n == new.n) { 
		runner->next->content.origin = NULL; 
	}
	// Cas pas encore dans la liste
	else {
		nodenew->next = runner->next;
		runner->next = nodenew;
	}

	return(EXIT_SUCCESS);
}
*/
