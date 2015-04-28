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
        debug_printf("In push\n");
        struct node * n = (struct node *) malloc(sizeof(struct node));
        debug_printf("In push, after malloc\n");
        if(n == NULL) {
                debug_printf("Malloc failed in push.\n");
                exit(EXIT_FAILURE);
        }

        n->content = new;
        n->next = *stack;
        *stack = n;
}

bool pop(struct node ** stack, struct number * popped)
{
        debug_printf("In pop\n");

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
                printf("%u - ", t->content.n);
                t = t->next;
        }
        
        printf("\n");

	debug_printf("Display: success!\n");
}

bool is_empty(struct node * stack)
{
        return(stack == NULL);
}
