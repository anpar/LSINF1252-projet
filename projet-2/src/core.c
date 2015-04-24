#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "core.h"
#include "stack.h"

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
                display(buffer1);
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


int insert(struct number *new)
{
	// Create a structure number that can be inserted
	struct number *toAdd = (struct number *) malloc(sizeof(struct number));
	if(toAdd == NULL)
		exit(EXIT_FAILURE);
	toAdd->n = new->n;
	toAdd->origin = new->origin;

	//Create a node in order to check the list node by node
	struct node *runner;
	runner = list;

	//Create the node containing the number that will be inserted
	struct node *nodenew;
	nodenew->content = toAdd;	

	// If the list is EMPTY
	if(list == NULL) {
		list = nodenew;
	}

	// If new has to be inserted as the FIRST NODE
	if(runner->content->n > new->n) { 
		nodenew->next = runner;
		list = nodenew;
	}

	// Run in the list until runner is just BEFORE
	//where it has to be inserted (or at the end)
	while(runner->next != NULL && runner->next->content->n < new->n) {
		runner = runner->next;	
	}

	// If new has to be inserted as the LAST NODE
	if(runner->next == NULL) { 
		nodenew->next = NULL;
		runner->next = nodenew;
	}

	// Inside the list, if new->n is ALREADY IN THE LIST
	else if(runner->next->content->n == new->n) { 
		runner->next->content->origin = NULL; 
	}

	// Inside the list, if new->n ISN'T IN THE LIST YET
	else {
		nodenew->next = runner->next;
		runner->next = nodenew;
	}

	return(EXIT_SUCCESS);
}


void free_list()
{
	while(list != NULL) {
		pop(&list);
	}
}

struct number * find_unique()
{
	//Create a node in order to check the list node by node
	struct node *runner;
	runner = list;

	while(runner->next != NULL && runner->content->origin == NULL) {
		runner = runner->next;
	}

	if(runner->content->origin != NULL) 
		return(runner->content);
		
	exit(EXIT_FAILURE);
}
