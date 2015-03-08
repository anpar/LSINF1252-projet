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
    t = (struct move *) malloc(sizeof(struct move));
    if(t == NULL) {
        return(-1);
    }
    else {
        t->seq = move;
        t->next = game->moves;
        game->moves = t;
        free(t);
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
            loaded_state->board[y][x] = board[y][x];
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
    // NOTE : valgrind indique un invalid free ici, je le mets temporairement
    // en commentaire pour voir (valgrind préfère comme, il faut comprendre
    // pourquoi. D'un autre côté, valgrind indique aussi une perte de 16 bytes ici.
    // En remettant cette ligne-ci on constate cependant que la perde n'est pas résolue.
    //free(game->moves);
    free(game);
    // frees the memory used for the structure containing the game
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
    struct move_seq *previous = NULL;

    // Tant que la liste de moves n'est pas vide
    while(runner != NULL) {
        struct coord *taken = (struct coord *) malloc(sizeof(struct coord));
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
                push(game, runner->seq);
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

int undo_moves(struct game *game, int n) {
	int count = 0;
	while((game->moves != NULL) && (count < n)) {
		pop(&(game->moves));
	}
	return(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct game *state = new_game(10,10);
    printf("C'est au tour du joueur %d.\n", state->cur_player);
    print_board(state);

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
    move_seq3.next = NULL;
    struct move mouvement3 = {&mouvement4, &move_seq3};

    /*
        Mouvement 2
    */
    struct coord c_old2 = {4,3};
    struct coord c_new2= {5,4};
    struct move_seq move_seq2;
    move_seq2.c_new = c_new2;
    move_seq2.c_old = c_old2;
    move_seq2.next = NULL;
    struct move mouvement2 = {&mouvement3, &move_seq2};

    /*
        Mouvement 1
    */
    struct coord c_old1 = {3,6};
    struct coord c_new1= {4,5};
    struct move_seq move_seq1;
    move_seq1.c_new = c_new1;
    move_seq1.c_old = c_old1;
    move_seq1.next = NULL;
    struct move mouvement1 = {&mouvement2, &move_seq1};
    int apply_moves_result1 = apply_moves(state, &mouvement1);
    print_board(state);

    /*
        Affichons game->move pour voir ce que ça dit
    */
    printf("Adresse du premier mouvement : %p.\n", state->moves);
    printf("Adresse de la première séquence du premier mouvement : %p.\n", state->moves->seq);
    printf("Déplacement de (%d,%d) à (%d,%d).\n", state->moves->seq->c_old.x, state->moves->seq->c_old.y, state->moves->seq->c_new.x, state->moves->seq->c_new.y);
    printf("Adresse de la deuxième séquence du premier mouvement : %p.\n", state->moves->seq->next);
    struct move_seq *pop1 = pop(&(state->moves));
    printf("Adresse de l'avant-dernier mouvement : %p.\n", state->moves);
    // NOTE : la dernière ligne retourne (nil). ça prouve selon moi qu'il y a un problème avec
    // les fonctions push/pop.

    free_game(state);
}
