#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

#define BLACK_PAWN 1
#define WHITE_PAWN 5
#define BLACK_QUEEN 3
#define WHITE_QUEEN 7
#define EMPTY_CASE 0

/*
 * ===================================================================
 *                         Added functions
 * ===================================================================
 */

/*
 * This function converts the byte reprensenting
 * the case on the board into a char and printf it.
 * - -blank character- is an empty case ;
 * - BP is a black plaw (opposite of WP) ;
 * - BQ is a black queen (opposite of WQ ;
 */
void print_case(int board_case) {
    if(board_case == BLACK_PAWN) {
        printf("   BP");
    }
    else if(board_case == WHITE_PAWN) {
        printf("   WP");
    }
    else if(board_case == BLACK_QUEEN) {
        printf("   BQ");
    }
    else if(board_case == WHITE_QUEEN) {
        printf("   WQ");
    }
    else {
        printf(" ");
    }
}


/*
    --------------------
            pop
    --------------------

    This function extracts the first element of the linked list
    of moves and returns its value. It frees the element too.
*/
struct move_seq *pop(struct move **list) {
    struct move *removed=*list; //adresse de list a liberer
    struct move_seq *value;
    value = removed->seq;
    *list=removed->next;
    free(removed);
    return(value);
}


/*
    ---------------------
            push
    ---------------------

    This function inserts the move move in
    the list of moves of game.
*/
int push(struct game *game, struct move_seq *move) {
    struct move *t;
    t = (struct move *)malloc(sizeof(struct move));
    if(t == NULL) {
        return(-1);
    }
    else {
        t->seq = move;
        t->next = game->moves;
        game->moves = t;
        return(0);
    }
}

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
 * ===================================================================
 *                       Requested functions
 * ===================================================================
 */

void print_board(const struct game *game) {
    int x,y;
    for(y = 0; y<game->ysize; y++) {
        for(x = 0; x<game->xsize; x++) {
            print_case(game->board[y][x]);
            if(x == game->xsize-1) {
                // Better to create a function that takes xsize as an argument and produce
                // a line corresponding to the size of tabulation * xize (but it's a detail)
                printf("\n----------------------------------------------------------------------------------\n");
            }
            else {
                printf(" \t|");
            }
        }
    }
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
                new_state->board[y][x] = 0;
            }
            // Black region
            else if (y < 4) {
                new_state->board[y][x] = 1;
            }
            // White zone
            else if (y > ysize - 5) {
                new_state->board[y][x] = 5;
            }
            // Empty cases
            else {
                new_state->board[y][x] = 0;
            }
    	}
    }
    return new_state;
}

/*
    --------------------
        load_game
    --------------------
*/
struct game *load_game(int xsize, int ysize, const int **board, int cur_player) {
    struct game *loaded_state = (struct game *) malloc(sizeof (struct game));
    if(loaded_state == NULL) {
	return(NULL);
    }
    loaded_state->xsize = xsize;
    loaded_state->ysize = ysize;
    loaded_state->cur_player = cur_player;
    loaded_state->moves = (struct move *) malloc(sizeof(struct move));
    loaded_state->moves->seq = NULL; // malloc ?
    loaded_state->board = (int **) malloc(ysize * sizeof(int *));
    int i;
    for (i = 0; i < ysize; i++) {
	loaded_state->board[i] = (int *) malloc(xsize * sizeof(int));
    }

    int x,y;
    for(x=0 ; x < xsize ; x++) {
        for(y=0 ; y < ysize ; y++) {
            loaded_state->board[x][y] = board[x][y];
            // This line must be checked :
            // as it's only a double pointer, I'm not sure of the syntax
        }
    }
    return loaded_state;
}

void free_game(struct game *game) {
    int i;
    for(i = 0 ; i < game->xsize ; i++) {
        //frees the memory used for the secondary tables in the primary table
        free(game->board[i]);
    }
    //frees the memory used for the primary table
    free(game->board);
    //  FREE MOVES TO DO
    free(game);
    //frees the memory used for the structure containing the game
}

//apply_move

/*
    --------------------
         apply_move
    --------------------

    This function applies a move list and inserts it in game->moves following this procedure :
    1) initializing of the runners or other variables
    2) checking the validity of a sequence
    3) applying the move on the game
    4) if a plaw has been taken :
    5) updating the board
    6) checking if the party is over (victory)
    7) checking if the sequence has ended
    8) if not : restart from 2) with the next element of the sequence
    if yes :
    9) checking the finale position of a plaw
    10) transform the plaw into a queen if needed
    11) changing the current player
    12) inserting the move in game->moves
    13) passing to the next element of the move list
    14) if not the end of the move list : restart from 2)
*/

/*int apply_moves(struct game *game, const struct move *moves) {
    struct move *runner;                  //parcourt la liste de moves
    struct move_seq *seq_runner;          //parcourt la sequence dans un move
    runner = moves;
    seq_runner = runner->seq;
    struct move_seq *previous = game->moves->seq;   //mouvement precedent d'une sequence
    previous = NULL;                                //initialement a NULL en debut de sequence

    while(runner != NULL) {      //Tant que la liste de moves n'est pas vide

        int validity;               //enregistre le resultat du test de validite
        struct coord *taken;        //enregistre les coord d'une piece prise (potentiellement)


        validity = is_move_seq_valid(game, seq_runner, previous, taken);

        if(validity == 0)          //mouvement invalide
            return (-1);
        else {                     //mouvement valide
            int oldx, oldy, newx, newy;
            oldx = seq_runner->c_old.x;
            oldy = seq_runner->c_old.y;
            newx = seq_runner->c_new.x;
            newy = seq_runner->c_new.y;
            game->board[newx][newy] = game->board[oldx][oldy];   //reajuste la position de la piece depacee
            game->board[oldx][oldy] = 0;

            if(runner->seq->next == NULL) {  //si la liste de sequences dans
                                             //l'element actuel de moves est terminee

                if(game->cur_player == PLAYER_BLACK) {   //tour du jourur noir
			if(newy == game->ysize-1 && game->board[newx][newy] == 1) {
				game->board[newx][newy] = 3;     //pion noir -> dame noire
			}
			game->cur_player = PLAYER_WHITE;     //chgt joueur -> blanc
		}
                else {                                   //tour du joueur noir
			if(newy == 0 && game->board[newx][newy] == 5) {
				game->board[newx][newy] = 7;     //pion blanc -> dame blanche
			}
			game->cur_player = PLAYER_BLACK;     //chgt joueur -> noir
		}

		push(game, runner->seq);  //insere le move parcouru dans game->moves
		runner = runner->next;    //passe a l'element suivant de moves
		seq_runner = runner->seq; //ajuste le seq_runner sur runner
		previous = NULL;          //reinitialise previous sur NULL (debut de sequence)

            }

            else {                          //si il reste des sequences dans
                                            //l'element actuel de moves

		previous = seq_runner,         //ajuste previous sur l'element analysé
                seq_runner = seq_runner->next; //passe a l'element suivant de la sequence
            }

            if(validity == 2) {        //si prise

                game->board[taken->x][taken->y] = 0;     //pliece prise devient vide
                int victory = checkVictory(game);        //CREER UNE FONCTION CHECKVICTORY !!!
                if(victory == 1)
                    return (1);
            }
        }
    }
}*/

/*
    NOTE : je m'étais dis quand une seule fonction c'était faisable
    de manière assez simple et propre, je n'en suis plus si sur. Je
    pense qu'il y a moyen de rendre ce code beaucoup plus propre et
    élégant. @Charles tu en dis quoi?
*/

int is_move_seq_valid(const struct game *game, const struct move_seq *seq, const struct move_seq *prev, struct coord *taken) {
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
    if((game->cur_player) != (game->board[y_old][x_old] & (1 << 2)) >> 2) {
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
        // @Charles tu confirmes?
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
            // Déplacement double vers l'avant et vers la droite
            else if(y_new-y_old == -2 && x_new-x_old == 2) {
                // On vérifie qu'il y bien prise d'un pièce adverse
                if(((game->board[y_old+1][x_old+1]) == BLACK_PAWN) || (((game->board[y_old+1][x_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[y_old+1][x_old-1]) == BLACK_PAWN) || (((game->board[y_old+1][x_old-1]) == BLACK_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite
            else if(y_new-y_old == 2 && x_new-x_old == 2) {
                if(((game->board[y_old-1][x_old+1]) == BLACK_PAWN) || (((game->board[y_old-1][x_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche
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
            // Déplacement double vers l'avant et vers la droite
            else if(y_new-y_old == 2 && x_new-x_old == 2) {
                if(((game->board[y_old+1][x_old+1]) == WHITE_PAWN) || (((game->board[y_old+1][x_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche
            else if(y_new-y_old == 2 && x_new-x_old == -2) {
                if(((game->board[y_old+1][x_old-1]) == WHITE_PAWN) || (((game->board[y_old+1][x_old-1]) == WHITE_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite
            else if(y_new-y_old == -2 && x_new-x_old == 2) {
                if(((game->board[y_old-1][x_old+1]) == WHITE_PAWN) || (((game->board[y_old-1][x_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[y_old-1][x_old-1]) == WHITE_PAWN) || (((game->board[y_old-1][x_old-1]) == WHITE_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
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

        // Il faut maintenant vérifier si la dame n'est pas passé sur un pion
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

int undo_moves(struct game *game, int n) {
	int count = 0;
	while((game->moves != NULL) && (count < n)) {
		pop(&(game->moves));
	}
	return(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
        struct game *state = new_game(10,10);
    print_board(state);

	struct move_seq *HUM;
	HUM = (struct move_seq *)malloc(sizeof (struct move_seq));
	struct coord old;
	old.x = 1;
	old.y = 1;
	struct coord new;
	new.x = 2;
	new.y = 2;
	HUM->next = NULL;
	HUM->c_old = old;
	HUM->c_new = new;
	HUM->piece_value = 0;
	HUM->piece_taken = old;
	HUM->old_orig = 0;

	if(state->moves->seq == NULL) {
	printf("seq vide\n");
	}
	else {printf("seq pas vide, c_old : %d,%d ; c_new : %d;%d \n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);}

	push(state, HUM);
	printf("push done\n");

	if(state->moves->seq == NULL) {
	printf("seq vide\n");
	}
	else {printf("seq pas vide, c_old : %d,%d ; c_new : %d;%d \n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);}

	struct move_seq *popped;
	popped = pop(&(state->moves));

	printf("apres pop\n");
	if(state->moves->seq == NULL) {
	printf("seq vide\n");
	}
	else {printf("seq pas vide, c_old : %d,%d ; c_new : %d;%d \n",state->moves->seq->c_old.x,state->moves->seq->c_old.y,state->moves->seq->c_new.x,state->moves->seq->c_new.y);}
	printf("popped : (%d,%d) -> (%d,%d) \n", popped->c_old.x, popped->c_old.y, popped->c_new.x, popped->c_new.y);

	int vic;
	vic = checkVictory(state);
	if(vic == 1) {printf("Virctoire de %d ! \n",state->cur_player);}
	else {printf("jeu tjs en cours\n");}

	struct game *state2 = new_game(10,10);
	int i,j;
	for(i=0 ; i < state2->xsize-1 ; i++) {
		for(j=0 ; j<state2->ysize-1 ; j++) {
			if(state2->board[i][j] == 1) {state2->board[i][j] = 0;}
		}
	}

	int vic2;
	vic2 = checkVictory(state2);
	if(vic2 == 1) {printf("Virctoire (pour state2) de %d ! \n",state2->cur_player);}
	else {printf("jeu tjs en cours (pour state2)\n");}

	struct game *state3 = load_game(10,10,state2->board,1);
	int vic3;
	vic3 = checkVictory(state2);
	if(vic3 == 1) {printf("Victoire (pour state3) de %d ! \n",state3->cur_player);}
	else {printf("jeu tjs en cours (pour state3)\n");}

    return(EXIT_SUCCESS);
}
