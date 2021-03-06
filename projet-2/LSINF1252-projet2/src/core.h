#ifndef _CORE_H
#define _CORE_H
#include <pthread.h>
#include <inttypes.h>

/* 
 * n is a number and origin is the name
 * of the file where this factor was found.
 * 
 * When a structure of this type is added
 * to the global linked list of factor, origin
 * can have several meanings. If the factor
 * is still unique in the list, then origin
 * will contains the name of the file
 * where the factor was found. If the
 * factor is not unique, then origin will
 * be NULL.
 */
struct number {
	uint64_t n;
	char *  origin;
};

/*
 * This structure allows us to create
 * a linked list containing every prime
 * factors.
 *
 * We also use this structure for the stacks
 * representing the two buffers.
 */
struct node {
	struct number content;
	struct node *next;
};

/*
 * @input : a string containing a local filename.
 * 
 * This function extracts numbers contained
 * in the file, converts them from BigEndian
 * format to usual format and then places them
 * one by one into the first buffer.
 */
extern void * extract_file(void *);

/*
 * This function factorizes a number in the
 * first buffer and places every prime factor
 * in the second buffer.
 */
extern void * factorize(void *);

/*
 * This function moves prime factors from
 * the second buffer to the global linked list.
 */
extern void * save_data(void *);

/*
 * This sub-function of save_data simply
 * add a struct prime_factor in a linked
 * list.
 */
extern void insert(struct number *);

/*
 * This function frees the linked-list list 
 * by popping each of its nodes.
 */
void free_list(void);

/*
 * This function finds the unique prime factor
 * into the global linked list of factors.
 */
extern int find_unique(struct number *);

#endif

