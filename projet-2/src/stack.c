#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "core.h"       // Pour les variables globales buffer1 et buffer2
#include "stack.h"

#define DEBUG false
/* 
 * This macro requires c99.
 */
#define debug_printf(fmt, ...) \
                            do { if (DEBUG) printf(fmt,## __VA_ARGS__); } while (0)

void push(struct node ** stack, struct number new)
{
        struct node * n = (struct node *) malloc(sizeof(struct node));
        if(n == NULL)
                exit(EXIT_FAILURE);

        n->content = new;
        n->next = *stack;
        *stack = n;
}

bool pop(struct node ** stack, struct number * popped)
{
	if(*stack == NULL)
        	return(true);
	
        *popped = (*stack)->content;

	struct node * removed = *stack;
       	*stack = (*stack)->next;
	free(removed);
       	return(false);
}

void display(struct node * stack)
{
        struct node *t;
        t = stack;

        while(t != NULL) {
                printf("%" PRIu64 " - ", t->content.n);
                t = t->next;
        }
        
        printf("\n");
}

bool is_empty(struct node * stack)
{
        return(stack == NULL);
}
