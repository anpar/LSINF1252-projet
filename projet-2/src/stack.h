#ifndef _STACK_H
#define _STACK_H

/*
 * push a new struct number on a buffer.
 */
void push(struct node **, struct number *);

/*
 * pop a struct number from a buffer.
 */
struct number * pop(struct node **);

/*
 * display a buffer.
 */
void display(struct node *);


/*
 * check if the buffer is empty
 */
bool is_empty(struct node *);

#endif
