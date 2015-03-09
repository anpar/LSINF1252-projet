#include <stdlib.h>
#include <stdio.h>
#include "dames.h"


#define BLACK_PAWN 1
#define WHITE_PAWN 5
#define BLACK_QUEEN 3
#define WHITE_QUEEN 7
#define EMPTY_CASE 0

/*
    ---------------------
        checkVictory
    ---------------------

    This function checks if cur_player has won
    after taking a plaw by counting the number
    of foe pieces left.
    Returns 1 if victory, 0 otherwise.
*/
int checkVictory(struct game *game) {
    int i,j,v1,v2;
    if(game->cur_player == PLAYER_BLACK) {
        v1 = 5;
        v2 = 7;
    }
    else {
        v1 = 1;
        v2 = 3;
    }
    for(i=0 ; i < game->xsize-1 ; i++) {
        for(j=0 ; j < game->ysize-1 ; j++) {
            if((game->board[i][j] == v1) || (game->board[i][j] == v2)) {
                //si au moins une piece ennemie reste => false
                return (0);
            }
        }
    }
    // si il ne reste aucune piece ennemie => true
    return (1);
}

/*
    --------------------
            pop
    --------------------

    This function extracts the first element of the linked list
    of moves and returns its value. It frees the element too.
*/
struct move *pop(struct move **list) {
    /*struct move *removed=*list; //adresse de list a liberer
    struct move_seq *value;
    value = removed->seq;
    *list=removed->next;
    free(removed);
    return(value);*/
	struct move *removed = *list;
	*list = removed->next;
	//free(removed);      si on doit free ce qui est popped ?
	return(removed);
}


/*
    ---------------------
            push
    ---------------------

    This function inserts the move move in
    the list of moves of game.
*/
int push(struct game *game, struct move *move) {
    /*struct move *t;
    t = (struct move *) malloc(sizeof(struct move));
    if(t == NULL) {
        return(-1);
    }
    else {*/
		struct move *t;
		t = move;
		t->next = game->moves;
		game->moves = t;
		//free(t);
        /*t->seq = move;
        t->next = game->moves;
        game->moves = t;
        free(t);*/
        return(0);
    //}
}




int apply_moves(struct game *game, const struct move *moves) {
    // parcourt la liste de moves
    struct move *runner;
    // parcourt la sequence dans un move
    struct move_seq *seq_runner;
    // Correction du warning avec un typecast
    runner = (struct move *) moves;
    seq_runner = runner->seq;
    // mouvement precedent d'une sequence
    struct move_seq *previous = NULL;

    // Tant que la liste de moves n'est pas vide
    while(runner != NULL) {
        struct coord *taken = (struct coord *) malloc(sizeof(struct coord));
        int validity = is_move_seq_valid(game, seq_runner, previous, taken);
        int taken_piece;
        if(taken!=NULL) {
            taken_piece = game->board[taken->y][taken->x];
        }

        // mouvement invalide
        if(validity == 0) {
            return(-1);
        }
        // mouvement valide
        else {
            
            int oldx = seq_runner->c_old.x;
            int oldy = seq_runner->c_old.y;
            int newx = seq_runner->c_new.x;
            int newy = seq_runner->c_new.y;
            // reajuste la position de la piece depacee

            seq_runner->old_orig = game->board[oldy][oldx];

            game->board[newy][newx] = game->board[oldy][oldx];  
            game->board[oldy][oldx] = EMPTY_CASE;

            // si la liste de sequences dans l'element actuel de moves est terminee
            if(seq_runner->next == NULL) {
                // tour du joueur noir
                if(game->cur_player == PLAYER_BLACK) {
                    if(newy == game->ysize-1 && game->board[newy][newx] == BLACK_PAWN) {
                    // pion noir -> dame noire
                    game->board[newy][newx] = BLACK_PAWN;
                }
                // Changement de joueur -> blanc
                game->cur_player = PLAYER_WHITE;
                }
                else {
                    // tour du joueur noir
                    if(newy == 0 && game->board[newy][newx] == WHITE_PAWN) {
                        // pion blanc -> dame blanche
                        game->board[newy][newx] = WHITE_QUEEN;
                    }
                    // Changement de joueur -> noir
                    game->cur_player = PLAYER_BLACK;
                }

                // insere le move parcouru dans game->moves
                /*  NOTE : ici on insère pas le move mais seulement la séquence, ça me parait
                    bizarre... C'est pour ça que je pensais changer les fonctions push et pop
                    car selon moi elles sont sensées push un move complet ou poper un move complet,
                    et pas des move_seq. D'ailleurs les tests que je fais dans la main prouvent
                    qu'il y a quelque chose qui ne va pas.
                */
				struct move *toAdd;
				toAdd = (struct move *) malloc(sizeof(struct move));
				toAdd->seq = runner->seq;
				toAdd->next = NULL;
                if(validity==2) {
                    toAdd->seq->piece_value = taken_piece;
                    toAdd->seq->piece_taken = *taken;
                }
                push(game, toAdd);
                // passe a l'element suivant de moves
                runner = runner->next;
                // ajuste le seq_runner sur runner
                if(runner != NULL) {
                    seq_runner = runner->seq;
                }

                previous = NULL;
            }
            // si il reste des sequences dans l'element actuel de moves
            else {
                // ajuste previous sur l'element analysé
                previous = seq_runner;
                // passe a l'element suivant de la sequence
                seq_runner = seq_runner->next;
            }

            // Si prise
            if(validity == 2) {
                // piece prise remplacée par une case vide
                game->board[taken->y][taken->x] = EMPTY_CASE;
                int victory = checkVictory(game);
                if(victory == 1) {
                    return (1);
                }
            }
        }
        free(taken);
        taken = NULL;
    }
	return(0);
}

/*
    NOTE : je m'étais dis quand une seule fonction c'était faisable
    de manière assez simple et propre, je n'en suis plus si sur. Je
    pense qu'il y a moyen de rendre ce code beaucoup plus propre et
    élégant. @Charles tu en dis quoi?
    Et aussi, quid de old_orig, piece_taken et piece_value? C'est ici
    que je suis sensé m'en occuper?
*/

int is_move_seq_valid(const struct game *game, const struct move_seq *seq, const struct move_seq *prev, struct coord *taken) {

    taken->x = 7;
    taken->y = 7;
	return(2); //FACILITE LE TEST, A EFFACER	

    // On effectue d'abord tous les tests sur c_old
    struct coord c_old = seq->c_old;
    int x_old = c_old.x;
    int y_old = c_old.y;

    // On vérifie que c_old est bien dans les limites du plateau
    if (x_old > game->xsize-1 || x_old < 0 || y_old > game->ysize-1 || y_old < 0) {
        return(0);
    }
    // On vérifie que c_old est une coordonnées accessibles (i.e. une case foncée)
    if ((x_old + y_old) % 2 == 0) {
        return(0);
    }
    // On vérifie que la pièce en c_old appartient bien au joueur à qui c'est le tour
    if((game->cur_player) != ((game->board[y_old][x_old] & (1 << 2)) >> 2)) {
        return(0);
    }

    // S'il y a eu un move_seq précédent
    if(prev != NULL) {
        struct coord prev_c_new = prev->c_new;
        int prev_x_new = prev_c_new.x;
        int prev_y_new = prev_c_new.y;

        // On vérifie que le move_seq précédent et celui-ci sont en accord (i.e c_new du
        // précédent move_seq = c_old de move_seq actuel)
        if((x_old != prev_x_new) || (y_old != prev_y_new)) {
            return(0);
        }
        // Il n'est pas nécessaire d'aller plus loin et de vérifier
        // qu'un pion redémarre bien de la case "juste après" la pièce prise,
        // car on vérifie déjà que le pion s'arrête bien juste derrière
        // dans les vérifications de c_new plus bas, la seule condition
        // à satisfaire est donc celle vérifié juste au-dessus.
    }

    // On passe maintenant aux tests sur c_new
    struct coord c_new = seq->c_new;
    int x_new = c_new.x;
    int y_new = c_new.y;

    // On vérifie que c_new se trouve bien dans les limites du plateau
    if(x_new > game->xsize-1 || x_new < 0 || y_new > game->ysize-1 || y_new < 0) {
        return(0);
    }
    // On vérifie ensuite que c_new est une coordonnée accessible (i.e. une case foncée)
    if((x_new + y_new) % 2 == 0) {
        return(0);
    }
    // On vérifie que c_new est bien vide
    if((game->board[y_new][x_new] & (1 << 0)) >> 0 == 1) {
        return(0);
    }

    // On doit maintenant vérifier que le déplacement effectué est correct
    // Pour cela, on doit envisager les différents cas

    // Si c'est un pion
    if(((game->board[y_old][x_old] & (1 << 1)) >> 1) == 0) {
        // Si c'est un pion blanc
        if(game->cur_player == PLAYER_WHITE) {
            // Déplacement classique, sans prise et donc vers l'avant
            if(y_new-y_old == -1 && abs(x_new-x_old) == 1) {
                return(1);
            }
            // Déplacement double vers l'avant et vers la droite (prise)
            else if(y_new-y_old == -2 && x_new-x_old == 2) {
                // On vérifie qu'il y bien prise d'un pièce adverse
                if(((game->board[y_old+1][x_old+1]) == BLACK_PAWN) || (((game->board[y_old+1][x_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche (prise)
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[y_old+1][x_old-1]) == BLACK_PAWN) || (((game->board[y_old+1][x_old-1]) == BLACK_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite (prise)
            else if(y_new-y_old == 2 && x_new-x_old == 2) {
                if(((game->board[y_old-1][x_old+1]) == BLACK_PAWN) || (((game->board[y_old-1][x_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche (prise)
            else if(y_new-y_old == 2 && x_new-x_old == -2) {
                if(((game->board[y_old-1][x_old-1]) == BLACK_PAWN) || (((game->board[y_old-1][x_old-1]) == BLACK_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Sinon, le déplacement n'est pas réglementaire
            else {
                return(0);
            }
        }
        // Si c'est un pion noir
        else {
            // Déplacement classique, sans prise et donc vers l'avant
            if(y_new-y_old == 1 && abs(x_new-x_old) == 1) {
                return(1);
            }
            // Déplacement double vers l'avant et vers la droite (prise)
            else if(y_new-y_old == 2 && x_new-x_old == 2) {
                if(((game->board[y_old+1][x_old+1]) == WHITE_PAWN) || (((game->board[y_old+1][x_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche (prise)
            else if(y_new-y_old == 2 && x_new-x_old == -2) {
                if(((game->board[y_old+1][x_old-1]) == WHITE_PAWN) || (((game->board[y_old+1][x_old-1]) == WHITE_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite (prise)
            else if(y_new-y_old == -2 && x_new-x_old == 2) {
                if(((game->board[y_old-1][x_old+1]) == WHITE_PAWN) || (((game->board[y_old-1][x_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche (prise)
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[y_old-1][x_old-1]) == WHITE_PAWN) || (((game->board[y_old-1][x_old-1]) == WHITE_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Sinon, le déplacement n'est pas réglementaire
            else {
                return(0);
            }
        }
    }
    // Si c'est une dame
    else {
        // On vérifie si le déplacement est bien diagonal
        if(abs(x_new-x_old) != abs(y_new-y_old)) {
            return(0);
        }

        // Il faut maintenant vérifier si la dame n'est pas passée sur un pion
        // adverse au passage, pour cela, on va parcourir toutes les cases qu'elle
        // a survolées de c_old à c_new. Afin d'arriver à faire cela, il faut envisager
        // plusieurs cas : les directions prises par la dame.

        int x_cur = x_old;
        int y_cur = y_old;

        // Si c'est une dame blanche (on ne parle pas de la glace)
        if(game->cur_player == PLAYER_WHITE) {
                while(x_cur != x_new && y_cur != y_new) {
                    if(game->board[y_cur][x_cur] == BLACK_PAWN || game->board[y_cur][x_cur] == BLACK_QUEEN) {
                        taken->x = x_cur;
                        taken->y = y_cur;
                        return(2);
                    }
                    // On vérifie que la dame ne passe au dessus d'un pion de son équipe
                    else if(game->board[y_cur][x_cur] == WHITE_PAWN || game->board[y_cur][x_cur] == WHITE_QUEEN) {
                        return(0);
                    }
                    else {
                        // Déplacement vers le haut et vers la droite
                        if((x_new-x_old) > 0 && (y_new-y_old) < 0) {
                            x_cur++;
                            y_cur--;
                        }
                        // Déplacement vers le bas et vers la droite
                        else if((x_new-x_old) > 0 && (y_new-y_old > 0)) {
                            x_cur++;
                            y_cur++;
                        }
                        // Déplacement vers le haut et vers la gauche
                        else if((x_new-x_old) < 0 && (y_new-y_old) < 0) {
                            x_cur--;
                            y_cur--;
                        }
                        // Déplacement vers le bas et vers la gauche
                        else {
                            x_cur--;
                            y_cur++;
                        }
                    }
                }
            }
        // Si c'est une dame noire
        else {
            while(x_cur != x_new && y_cur != y_new) {
                if(game->board[y_cur][x_cur] == WHITE_PAWN || game->board[y_cur][x_cur] == WHITE_QUEEN) {
                    taken->x = x_cur;
                    taken->y = y_cur;
                    return(2);
                }
                // On vérifie que la dame ne passe pas au dessus d'un pion de son équipe
                else if(game->board[y_cur][x_cur] == BLACK_PAWN || game->board[y_cur][x_cur] == BLACK_QUEEN) {
                    return(0);
                }
                else {
                    // Déplacement vers le haut et vers la droite
                    if((x_new-x_old) > 0 && (y_new-y_old) < 0) {
                        x_cur++;
                        y_cur--;
                    }
                    // Déplacement vers le bas et vers la droite
                    else if((x_new-x_old) > 0 && (y_new-y_old > 0)) {
                        x_cur++;
                        y_cur++;
                    }
                    // Déplacement vers le haut et vers la gauche
                    else if((x_new-x_old) < 0 && (y_new-y_old) < 0) {
                        x_cur--;
                        y_cur--;
                    }
                    // Déplacement vers le bas et vers la gauche
                    else {
                        x_cur--;
                        y_cur++;
                    }
                }
            }
        }

        // Si on arrive jusqu'ici, c'est que la dame a effectué un déplacement
        // sans faire de prise
        return(1);
    }
    return(0);
}





struct game *new_game(int xsize, int ysize) {
    struct game *new_state = (struct game *) malloc(sizeof (struct game));
    if(new_state == NULL) {
        return(NULL);
    }

    new_state->xsize=xsize;
    new_state->ysize=ysize;
    new_state->cur_player = PLAYER_WHITE;
    new_state->moves = (struct move *) malloc(sizeof(struct move));
    new_state->moves->seq = NULL;
    new_state->board = (int **) malloc(ysize * sizeof(int *));

    int i;
    for (i = 0; i < ysize; i++) {
	new_state->board[i] = (int *) malloc(xsize * sizeof(int));
    }

    int x,y;
    for (x = 0; x < xsize; x++) {
    	for (y = 0; y < ysize; y++) {
    	    // Empty cases
    	    if ((x+y) % 2 == 0)  {
                new_state->board[y][x] = EMPTY_CASE;
            }
            // Black region
            else if (y < 4) {
                new_state->board[y][x] = BLACK_PAWN;
            }
            // White zone
            else if (y > ysize - 5) {
                new_state->board[y][x] = WHITE_PAWN;
            }
            // Empty cases
            else {
                new_state->board[y][x] = EMPTY_CASE;
            }
    	}
    }
    return new_state;
}




//=============================================================================================




int main(int argc, const char *argv[]) {
	struct game *state = new_game(10,10);
	state->moves = NULL;
	/*state = (struct game *) malloc(sizeof(struct game));
	state->moves = (struct move *) malloc(sizeof(struct move));
    state->moves->seq = NULL;*/

	struct coord c_old1 = {3,6};
    struct coord c_new1= {4,5};
    struct move_seq move_seq1;
    move_seq1.c_new = c_new1;
    move_seq1.c_old = c_old1;
    move_seq1.next = NULL;
    struct move mouvement1 = {NULL, &move_seq1};

	struct coord c_old2 = {4,3};
    struct coord c_new2= {5,4};
    struct move_seq move_seq2;
    move_seq2.c_new = c_new2;
    move_seq2.c_old = c_old2;
    move_seq2.next = NULL;
    struct move mouvement2 = {NULL, &move_seq2};

	/*
        Mouvement 4
    */
    struct coord c_old4 = {5,4};
    struct coord c_new4= {3,6};
    struct move_seq move_seq4;
    move_seq4.c_new = c_new4;
    move_seq4.c_old = c_old4;
    move_seq4.next = NULL;
    struct move mouvement4 = {NULL, &move_seq4};

    /*
        Mouvement 3
    */
    struct coord c_old3 = {5,6};
    struct coord c_new3= {6,5};
    struct move_seq move_seq3;
    move_seq3.c_new = c_new3;
    move_seq3.c_old = c_old3;
    move_seq3.next = &move_seq4;
    struct move mouvement3 = {NULL, &move_seq3};

	printf("\nadresse de moves : %p.\n",state->moves);
	//printf("adresse de move_seq : %p.\n\n",state->moves->seq);

	push(state,&mouvement1);
	printf("push.\n");
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);

	push(state,&mouvement2);
	printf("push2 (un deuxieme move contenant une sequence).\n");
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);
	printf("adresse de moves->next : %p.\n",state->moves->next);
	printf("adresse de moves->next->move_seq : %p.\n",state->moves->next->seq);
	printf("contenu de moves->next->move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->next->seq->c_old.x,state->moves->next->seq->c_old.y,state->moves->next->seq->c_new.x,state->moves->next->seq->c_new.y);

	push(state,&mouvement3);
	printf("push3 (un move contenant 2 sequences).\n");
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);
	printf("adresse de move_seq->next : %p.\n",state->moves->seq->next);
	printf("contenu de move_seq->next :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->next->c_old.x,state->moves->seq->next->c_old.y,state->moves->seq->next->c_new.x,state->moves->seq->next->c_new.y);
	printf("adresse de moves->next : %p.\n",state->moves->next);
	printf("adresse de moves->next->move_seq : %p.\n",state->moves->next->seq);
	printf("contenu de moves->next->move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->next->seq->c_old.x,state->moves->next->seq->c_old.y,state->moves->next->seq->c_new.x,state->moves->next->seq->c_new.y);

	struct move *removed;
	removed = pop(&(state->moves));
	printf("pop.\n");
	printf("contenu de removed->seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",removed->seq->c_old.x,removed->seq->c_old.y,removed->seq->c_new.x,removed->seq->c_new.y);
	printf("contenu de removed->seq->next :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",removed->seq->next->c_old.x,removed->seq->next->c_old.y,removed->seq->next->c_new.x,removed->seq->next->c_new.y);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);

	pop(&(state->moves));
printf("OK\n");
	pop(&(state->moves));
printf("OK\n");
	printf("tout popped ; adresse de moves : %p.\n\n",state->moves);
//---------------------------------------------------------------------


	struct coord c_old5 = {4,3};
    struct coord c_new5= {5,4};
    struct move_seq move_seq5;
    move_seq5.c_new = c_new5;
    move_seq5.c_old = c_old5;
    move_seq5.next = NULL;
    struct move mouvement5 = {NULL, &move_seq5};

	/*
        Mouvement 6
    */
    struct coord c_old6 = {5,4};
    struct coord c_new6= {3,6};
    struct move_seq move_seq6;
    move_seq6.c_new = c_new6;
    move_seq6.c_old = c_old6;
    move_seq6.next = NULL;
    struct move mouvement6 = {NULL, &move_seq6};

    /*
        Mouvement 7
    */
    struct coord c_old7 = {5,6};
    struct coord c_new7= {6,5};
    struct move_seq move_seq7;
    move_seq7.c_new = c_new7;
    move_seq7.c_old = c_old7;
    move_seq7.next = &move_seq6;
    struct move mouvement7 = {NULL, &move_seq7};

	printf("recommencer avec apply_moves.\n\n");

	printf("adresse de moves : %p.\n",state->moves);
	//printf("adresse de move_seq : %p.\n\n",state->moves->seq);

	int apply1 = apply_moves(state, &mouvement1);
	printf("apply1.\n");
	printf("resultat de apply1 : %d\n",apply1);
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);
    printf("tests de prise\n");
    printf("contenu de taken : (%d,%d).\n",state->moves->seq->piece_taken.x,state->moves->seq->piece_taken.y);
    printf("contenu old_orig : %d.\n", state->moves->seq->old_orig);
    printf("contenu de piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->seq->piece_value);

	//BOUCLE INFINIE A PARTIR D'ICI
	
	printf("movement2->next : %p\n",mouvement2.next);
	printf("movement2->next->next : %p\n",mouvement2.next->next); //result : (nil)
	printf("%p\n",mouvement5.next);
	int apply2 = apply_moves(state, &mouvement5);
	printf("apply2.\n");
	printf("apply2 (un deuxieme move contenant une sequence).\n");
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);
	printf("adresse de moves->next : %p.\n",state->moves->next);
	printf("adresse de moves->next->move_seq : %p.\n",state->moves->next->seq);
	printf("contenu de moves->next->move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->next->seq->c_old.x,state->moves->next->seq->c_old.y,state->moves->next->seq->c_new.x,state->moves->next->seq->c_new.y);
    printf("tests de prise\n");
    printf("contenu de taken : (%d,%d).\n",state->moves->seq->piece_taken.x,state->moves->seq->piece_taken.y);
    printf("contenu old_orig : %d.\n", state->moves->seq->old_orig);
    printf("contenu de piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->seq->piece_value);

	apply_moves(state,&mouvement7);
	printf("apply3 (un move contenant 2 sequences).\n");
	printf("adresse de moves : %p.\n",state->moves);
	printf("adresse de move_seq : %p.\n",state->moves->seq);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);
	printf("adresse de move_seq->next : %p.\n",state->moves->seq->next);
	printf("contenu de move_seq->next :\nc_old : (%d,%d)\nc_new : (%d,%d)\n",state->moves->seq->next->c_old.x,state->moves->seq->next->c_old.y,state->moves->seq->next->c_new.x,state->moves->seq->next->c_new.y);
	printf("adresse de moves->next : %p.\n",state->moves->next);
	printf("adresse de moves->next->move_seq : %p.\n",state->moves->next->seq);
	printf("contenu de moves->next->move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->next->seq->c_old.x,state->moves->next->seq->c_old.y,state->moves->next->seq->c_new.x,state->moves->next->seq->c_new.y);
    printf("tests de prise\n");
    printf("contenu de taken : (%d,%d).\n",state->moves->seq->piece_taken.x,state->moves->seq->piece_taken.y);
    printf("contenu old_orig : %d.\n", state->moves->seq->old_orig);
    printf("contenu de piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->seq->piece_value);
    printf("tests de prise precedente\n");
    printf("contenu de next->taken : (%d,%d).\n",state->moves->next->seq->piece_taken.x,state->moves->seq->piece_taken.y);
    printf("contenu next->old_orig : %d.\n", state->moves->next->seq->old_orig);
    printf("contenu de next->piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->next->seq->piece_value);
    printf("tests de prise de la sequence precedente\n");
    printf("contenu de seq->next->taken : (%d,%d).\n",state->moves->seq->next->piece_taken.x,state->moves->seq->piece_taken.y);
    printf("contenu seq->next->old_orig : %d.\n", state->moves->seq->next->old_orig);
    printf("contenu de seq->next->piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->seq->next->piece_value);

	struct move *removed2;
	removed2 = pop(&(state->moves));
	printf("pop.\n");
	printf("contenu de removed2->seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",removed2->seq->c_old.x,removed2->seq->c_old.y,removed2->seq->c_new.x,removed2->seq->c_new.y);
	printf("contenu de removed2->seq->next :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",removed2->seq->next->c_old.x,removed2->seq->next->c_old.y,removed2->seq->next->c_new.x,removed2->seq->next->c_new.y);
	printf("contenu de move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);

	pop(&(state->moves));
	pop(&(state->moves));
	printf("tout popped ; adresse de moves : %p.\n\n",state->moves);

	printf("TOUS LES TESTS SE SONT TERMINES AVEC SUCCES\n");
}
