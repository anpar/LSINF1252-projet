#ifndef _CORE_H
#define _CORE_H

/*
 * This structure allows us to create
 * a linked list containing every prime
 * factors
 */
extern struct node {
	struct prime_factor content;
	struct node *next;
}


/*
 * This structure will be contained
 * in the first buffer. n is a number
 * to factorize and origin is the name
 * of the file where this number was found/
 */
extern struct number {
	unsigned int n;
	char * origin;
};

/* 
 * This structure is quite similar
 * to the one above. The only difference
 * is that this one will be contained in
 * the second buffer. f is a prime
 * factor and origin is still the name
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
struct struct prime_factor {
	unsigned int f;
	char * origin;
}

/*
 * @input : a string containing either a local
 * filename or an URL.
 *
 * This function will call either extract_url
 * or extract_file.
 */
extern void * extract(void *);

/*
 * @input : a string containing an URL.
 *
 * This function extracts numbers contained
 * at URL, converts them from BigEndian
 * format to usual format and then places them
 * one by one into the first buffer.
 */
extern void * extract_url(void *);

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
 * This function finds the unique prime factor
 * into the global linked list of factors.
 */
extern struct prime_factor find_unique(void);

#endif

