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

int apply_moves(struct game *game, const struct move *moves) {
    // parcourt la liste de moves
    struct move *runner;
    // parcourt la sequence dans un move
    struct move_seq *seq_runner;
    // Correction du warning avec un typecast
    runner = (struct move *) moves;
    seq_runner = runner->seq;
    // mouvement precedent d'une sequence
    struct move_seq *previous = game->moves->seq;
    // initialement a NULL en debut de sequence
    // NOTE : du coup pourquoi l'initialiser à autre chose au dessus @Charles ?
    previous = NULL;

    // Tant que la liste de moves n'est pas vide
    while(runner != NULL) {
        struct coord *taken;
        int validity = is_move_seq_valid(game, seq_runner, previous, taken);

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
            // NOTE : j'ai un gros doute sur ça depuis le début. Mais est-ce
            // que ce n'est pas plutt board[y][x], le première indice y désigne les
            // lignes, et le deuxièmes x désigne les colonnes non? (en tout cas c'est comme ça
            // que j'ai fais pour print_board et new_game) Si c'est bien le cas,
            // il faudra inverser ici.
            game->board[newx][newy] = game->board[oldx][oldy];
            game->board[oldx][oldy] = EMPTY_CASE;

            // si la liste de sequences dans l'element actuel de moves est terminee
            if(runner->seq->next == NULL) {
                // tour du joueur noir
                if(game->cur_player == PLAYER_BLACK) {
                    if(newy == game->ysize-1 && game->board[newx][newy] == BLACK_PAWN) {
                    // pion noir -> dame noire
                    game->board[newx][newy] = BLACK_PAWN;
                }
                // Changement de joueur -> blanc
                game->cur_player = PLAYER_WHITE;
                }
                else {
                    // tour du joueur noir
                    if(newy == 0 && game->board[newx][newy] == WHITE_PAWN) {
                        // pion blanc -> dame blanche
                        game->board[newx][newy] = WHITE_QUEEN;
                    }
                    // Changement de joueur -> noir
                    game->cur_player = PLAYER_BLACK;
                }

                // insere le move parcouru dans game->moves
                push(game, runner->seq);
                // passe a l'element suivant de moves
                runner = runner->next;
                // ajuste le seq_runner sur runner
                seq_runner = runner->seq;
                // reinitialise previous sur NULL (debut de sequence)
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
                game->board[taken->x][taken->y] = EMPTY_CASE;
                int victory = checkVictory(game);
                if(victory == 1) {
                    return (1);
                }
            }
        }
    }
}

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
}
