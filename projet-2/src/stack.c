#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core.h"
#include "stack.h"

#define DEBUG false
/* 
 * This macro requires c99.
 */
#define debug_printf(fmt, ...) \
                            do { if (DEBUG) printf(fmt,## __VA_ARGS__); } while (0)

void push(struct node ** stack, struct number new)
{
        struct node *n = (struct node *) malloc(sizeof(struct node));
        if(n == NULL)
                exit(EXIT_FAILURE);
	
        n->content = new;
        n->next = *stack;
        *stack = n;
	
	debug_printf("Push: success!\n");
}

bool pop(struct node ** stack, struct number * popped)
{
	if(*stack == NULL)
        	return(true);
	
        *popped = (*stack)->content;

	struct node * removed = *stack;
       	*stack = (*stack)->next;
	free(removed);

       	debug_printf("Pop: success!\n");
       	return(false);
}

void display(struct node * stack)
{
        struct node *t;
        t = stack;
        
        printf("Buffer: ");

        while(t != NULL) {
                printf("%u (%s) - ", t->content.n, t->content.origin);
                t = t->next;
        }
        
        printf("\n");

	debug_printf("Display: success!\n");
}
