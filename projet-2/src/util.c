#include <stdio.h>
#include <stdlib>


/*
 * Une structure contenant un nombre et son fichier d'origine
 */
typedef struct prime_factor {
	unsigned int n;
	char *origin;
	} prime_factor;


/*
 * Liste chainée contenant des structures de type number
 */
typedef struct node {
	struct prime_factor content;
	struct node *next;
	} node;


int insert(struct prime_factor toAdd)
{
	struct node *runner;
	runner = list;

	if(runner->content.n > toAdd) { //cas debut de la liste
		struct node *nodeToAdd;
		nodeToAdd->content = toAdd;
		nodeToAdd->next = runner;
		runner = toAdd;
	}

	//boucle dans la liste jusqu'a etre juste AVANT l'endroit de l'insertion (ou au bout)
	while(runner->next != NULL && runner->next->content.n < toAdd.n) {
		runner = runner->next;	
	}
	if(runner->next == NULL) { //cas fin de la liste
		struct node *nodeToAdd;
		nodeToAdd->content = toAdd;
		nodeToAdd->next = NULL;
		runner->next = nodeToAdd;
	}
	else if(runner->next->content.n == toAdd) { //cas deja present dans la liste
		runner->next->content->origin = NULL; //l'origine est passée a null
	}
	else {//cas pas encore dans la liste
		struct node *nodeToAdd;
		nodeToAdd->content = toAdd;
		nodeToAdd->next = runner->next;
		runner->next = nodeToAdd;
	}
	return(EXIT_SUCCESS);
}


int main(int argc, int *argv[])
{
}
