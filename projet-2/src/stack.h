#ifndef _STACK_H
#define _STACK_H

/*
 * push a new struct number on a buffer.
 */
void push(struct node **, struct number);

/*
 * pop a struct number from a buffer and
 * make number point to this struct.
 * Additionnally, this functions returns
 * true if the stack is empty and false
 * either.
 */
bool pop(struct node **, struct number *);

/*
 * display a buffer.
 */
void display(struct node *);

#endif
