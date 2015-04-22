#include <stdio.h>
#include <stdlib.h>
#include "core.h"

struct node * list;

int insert(struct prime_factor new)
{
	struct node *runner;
	runner = list;
	struct node *nodenew;
	nodenew->content = new;	

	// Cas début de la liste
	if(runner->content.f > new.f) { 
		nodenew->next = runner;
		runner->content = new;
	}

	// Boucle dans la liste jusqu'a etre juste avant 
	// l'endroit de l'insertion (ou au bout)
	while(runner->next != NULL && runner->next->content.f < new.f) {
		runner = runner->next;	
	}

	// Cas fin de la liste
	if(runner->next == NULL) { 
		nodenew->next = NULL;
		runner->next = nodenew;
	}
	// Cas deja present dans la liste, l'origine est passée a null
	else if(runner->next->content.f == new.f) { 
		runner->next->content.origin = NULL; 
	}
	// Cas pas encore dans la liste
	else {
		nodenew->next = runner->next;
		runner->next = nodenew;
	}

	return(EXIT_SUCCESS);
}
