#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

int main(int argc, char *argv[]) {
    // TODO : interface

	printf("Jeu de dames sur console\nAuteurs : Monnoyer Charles, Paris Antoine\n \n");

	//Lancement d'une partie
	char init = 0;
	while((init != 'L') && (init != 'N')) 
	{
		printf("Pour initialiser un nouveau jeu, tapez 'N'.\nPour charger un jeu a partir d'un plateau existant, tapez 'L'.\n");
		scanf("%c",&init);		
	}
	if(init=='N') {
		int size;
		printf("Entrez la largeur du plateau.\n");
		scanf("%d",&size);
		print_board(new_game(size,size));
	}

    return(EXIT_SUCCESS);
}
