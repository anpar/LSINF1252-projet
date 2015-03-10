#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dames.h"


void reverse(struct move_seq **list) {
   struct move_seq *runner, *reversed, *trace; //runner parcourt la liste, 
                                           //tandis que reversed enregistre la liste inversee
                                           //et que trace retient reversed a chaque etape
   runner = *list;
   reversed = NULL;
   while(runner != NULL) {
      trace = reversed;  //trace enregistre la liste inversee intermediaire
      reversed = runner;  //reversed enregistre l'element suivant
      runner = runner->next;  //runner avance dans la liste
      reversed->next = trace;  //reversed complete l'element acquis avec 
                               //la liste inversee intermediaire
   } 
   *list = reversed;
}


int main(int argc, char *argv[]) {
    // TODO : interface

	printf("DAME BLANCHE\nJeu de dames sur console\nAuteurs : Monnoyer Charles, Paris Antoine\n \n");

	struct game* state;

//Lancement d'une partie
	char init = 0;
	while((init != 'L') && (init != 'N')) 
	{
		printf("Pour initialiser un nouveau jeu, tapez 'N'.\nPour charger un jeu a partir d'un plateau existant, tapez 'L'.\n");
		scanf("%c",&init);		
	}

	//si il s'agit d'un nouveau jeu
	if(init=='N') {
		int sizeL,sizeH;
		printf("Entrez la largeur du plateau (entre 10 et 20).\n");
		scanf("%d",&sizeL);
		printf("Entrez la hauteur du plateau (entre 10 et 20).\n");
		scanf("%d",&sizeH);
		while((sizeL>20) || (sizeL<0) || (sizeH>20) || (sizeH<0)) {  
			printf("Entrée invalide : %d,%d\n",sizeL,sizeH);
			printf("Entrez la largeur du plateau.\n");
			scanf("%d",&sizeL);
			printf("Entrez la hauteur du plateau.\n");
			scanf("%d",&sizeH);
		}
			printf("Voici le plateau obtenu :\n\n");
			state = new_game(sizeL,sizeH);
			print_board(state);  
			//NOTE : je ne sais pas ou enregistrer le plateau créé ; en effet, 
			//on ne peut ni l'enregistrer ici, ni dans dames.c je crois, non ?
	}

	//si il s'agit de charger un jeu
	else {
		int sizeL,sizeH;
		int **tab;
		int player = -1;
		printf("Entrez la largeur du plateau (entre 10 et 20).\n");
		scanf("%d",&sizeL);
		printf("Entrez la hauteur du plateau (entre 10 et 20).\n");
		scanf("%d",&sizeH);
		while((sizeL>20) || (sizeL<0) || (sizeH>20) || (sizeH<0)) {
			printf("Entrée invalide : %d,%d.\n",sizeL,sizeH);
			printf("Entrez la largeur du plateau.\n");
			scanf("%d",&sizeL);
			printf("Entrez la hauteur du plateau.\n");
			scanf("%d",&sizeH);
		}
		printf("Entrez l'adresse du pointeur contenant l'adresse du plateau existant.\n");
		scanf("%p", &tab);
		while((player != 1) && (player != 0)) {
			printf("Tapez '1' si c'est au tour du joueur noir, '0' pour le joueur blanc.\n");
			scanf("%d", &player);
		}
		printf("Voici le plateau obtenu :\n");
		if(player == 1) {
			printf("joueur : joueur noir.\n\n");
		}
		else {
			printf("joueur : joueur blanc.\n\n");
		}
		state = load_game(sizeL,sizeH,(const int **)tab,player);
		print_board(state);
		//NOTE : meme remarque que plus haut
	}

//DEBUT DE LA PARTIE

	int victoire = 0;
	char action;

	printf("Au tour du joueur %d (0 = noir, 1 = blanc).\n Que voulez-vous faire ?\n Tapez :\n",state->cur_player);
	printf("'P' pour afficher le plateau ;\n'M' pour faire un mouvement ;\n'U' pour annuler un mouvement ;\n'Q' pour quitter.\n\n");
	scanf("%c",&action);

	//tant que la partie n'est pas gagnée

	//NOTE : je ne sais pas pourquoi, mais si je ne mets pas un
	//scan avant le while, au début et a la fin ca fait des trucs bizarres...
	while((victoire != 1)) {
		scanf("%c",&action);

		if((action !='Q') && (action !='P') && (action!='M') && (action != 'U')) 
		{
			printf("Entrée invalide : %c\n\n", action);
		}

		//si on quitte la partie
		if(action == 'Q')
		{
			printf("Partie terminée, merci d'avoir joué.\n");
			return(EXIT_SUCCESS);
		}

		//si on veut afficher le plateau
		else if(action == 'P') 
		{
			printf("Plateau :\n\n");
			print_board(state);
			printf("\nPlateau imprimé.\n\n");
		}

		//si on veut effectuer un mouvement
		else if(action == 'M')
		{
/*			//variable définissant si le move est complet 
			int endOfMove = 0;
			//variable définissant si c'est la première séquence
			int first_seq = 0;
			//enregistre le resultat de apply_move
			int result;

			//variable contenant la sequence précédente du move à insérer
			struct move_seq *previous = NULL;
			//enregistre la premiere séquence de la liste de moves
			struct move_seq *first = NULL;

			//tant que le move n'est pas complètement défini
			while(endOfMove == 0) 
			{
				char yes_no; 
				//Définition des coordonnées de déplacement
				int old_x, old_y, new_x, new_y;
				printf("Quelle est la coordonée initiale (en largeur) du pion que vous voulez déplacer ?\n");
				scanf("%d",&old_x);
				printf("Quelle est la coordonée initiale (en hauteur) du pion que vous voulez déplacer ?\n");
				scanf("%d",&old_y);
				printf("Où souhaitez-vous le déposer (en largeur) ?\n");
				scanf("%d",&new_x);
				printf("Où souhaitez-vous le déposer (en hauteur) ?\n");
				scanf("%d",&new_y);

				//Définition de la séquence à insérer
				struct coord old = {old_x,old_y};
				struct coord new = {new_x,new_y};
				struct move_seq seq;
				seq.c_old = old;
				seq.c_new = new;
				seq.next = NULL;

				//enregistre la première séquence pour définir le move plus tard
				if(first_seq == 0)
				{
					first = &seq;
					first_seq = 1;
				}

				//enregistre la séquence précédent pour tout lier à la fin
				if(previous != NULL)
				{
					previous->next = &seq;
printf("OK\n");
				}
				previous = &seq;

				printf("Le mouvement est-il fini ? (O/N)\n");
				scanf("%c",&yes_no);
				while((yes_no != 'O') && (yes_no != 'N'))
				{
					scanf("%c",&yes_no);
					if((yes_no != 'O') && (yes_no != 'N'))
					{	printf("Entrée invalide : %c\n", yes_no);
						printf("Le mouvement est-il fini ? (O/N)\n");
					}
				}
				
				//si le move est terminé
				if(yes_no == 'O')
				{
					struct move *move = (struct move *) malloc(sizeof(struct move));
					move->seq = first;
					move->next = NULL;
					result = apply_moves(state, (const struct move *) move);
printf("APPLY DONE : %d\n", result);
					free(move);
					endOfMove = 1;
				}

				//sinon
				else
				{
					printf("Quelle est la suite du mouvement ?\n");
				}
			}//fin du while
			if(result == -1)
				{
					printf("Mouvement invalide ou non-réglementaire.\n\n");
				}
			else
				{
					victoire = result;
				}
		printf("Mouvement accompli.\n\n");
*/
			//variable définissant si le move est complet 
			int endOfMove = 0;
			//enregistre le resultat de apply_move
			int result;

			//variable contenant la sequence precedente
			struct move_seq *previous = NULL;
			
			while(endOfMove == 0)
			{
				char yes_no; 
				//Définition des coordonnées de déplacement
				int old_x, old_y, new_x, new_y;
				printf("Quelle est la coordonée initiale (en largeur) du pion que vous voulez déplacer ?\n");
				scanf("%d",&old_x);
				printf("Quelle est la coordonée initiale (en hauteur) du pion que vous voulez déplacer ?\n");
				scanf("%d",&old_y);
				printf("Où souhaitez-vous le déposer (en largeur) ?\n");
				scanf("%d",&new_x);
				printf("Où souhaitez-vous le déposer (en hauteur) ?\n");
				scanf("%d",&new_y);

				//Définition de la séquence à insérer
				struct coord old = {old_x,old_y};
				struct coord new = {new_x,new_y};
				struct move_seq *seq = (struct move_seq *) malloc(sizeof(struct move_seq));
				seq->c_old = old;
				seq->c_new = new;
				seq->next = previous;

				previous = seq;

				printf("Le mouvement est-il fini ? (O/N)\n");
				scanf("%c",&yes_no);
				while((yes_no != 'O') && (yes_no != 'N'))
				{
					scanf("%c",&yes_no);
					if((yes_no != 'O') && (yes_no != 'N'))
					{	printf("Entrée invalide : %c\n", yes_no);
						printf("Le mouvement est-il fini ? (O/N)\n");
					}
				}
				
				//si le move est terminé
				if(yes_no == 'O')
				{
					reverse(&seq);
//printf("coord en argument : (%d,%d)\n",seq->c_old.x,seq->c_old.y);
					struct move *move = (struct move *) malloc(sizeof(struct move));
					move->seq = seq;
					move->next = NULL;
					result = apply_moves(state, (const struct move *) move);
//printf("APPLY DONE : %d\n", result);
					free(move);
					endOfMove = 1;
				}

				//sinon
				else
				{
					printf("Quelle est la suite du mouvement ?\n");
				}
			}//fin du while
			if(result == -1)
				{
					printf("Mouvement invalide ou non-réglementaire.\n\n");
				}
			else
				{
					victoire = result;
				}
		printf("Mouvement accompli.\n\n");
		}

		//si on veut annuler un mouvement
		else
		{
			int undo_qtty;
			printf("Combien de mouvements voulez-vous annuler ? (max 50)\n");
			scanf("%d",&undo_qtty);
			while((undo_qtty < 0) || (undo_qtty > 50))
			{
				printf("Entrée invalide.\n");
				printf("Combien de mouvements voulez-vous annuler ? (max 50)\n");
			    scanf("%d",&undo_qtty);
			}
printf("undo reached, undo_qtty : %d\n\n", undo_qtty);
printf("adresse de game->moves (!= NULL si au moins un move effectué) : %p\n\n", state->moves);
			undo_moves(state, undo_qtty);
            print_board(state);
            printf("\nTableau corrigé.\n\n");

		}
	printf("Au tour du joueur %d (1 = blanc, 0 = noir).\n Que voulez-vous faire ?\n Tapez :\n",state->cur_player);
	printf("'P' pour afficher le plateau ;\n'M' pour faire un mouvement ;\n'U' pour annuler un mouvement ;\n'Q' pour quitter.\n\n");
	scanf("%c",&action);
	}  

//La partie est gagnée

    print_board(state);

    //par le joueur blanc
    if(state->cur_player == 1)
    {
        printf("LE JOUEUR BLNC GAGNE LA PARTIE !\n");
    }

    //par le joueur noir
    else
    {
        printf("LE JOUEUR NOIR GAGNE LA PARTIE !\n\n");
    }

    printf("Félicitations au gagnant et bien joué aux deux joueurs !\n Merci d'avoir joué à Dame Blanche, jeu de dames sur console !\n\n");

    return(EXIT_SUCCESS);
}
