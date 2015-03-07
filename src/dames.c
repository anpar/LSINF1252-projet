#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

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
 *
 * Note : may be more efficient and more elegant. This is just
 * a first version.
 */
void print_case(int byte) {
    // Extraction of the three first bits
    int first = (byte & (1 << 0)) >> 0;
    int second = (byte & (1 << 1)) >> 1;
    int third = (byte & (1 << 2)) >> 2;
    switch(first) {
	case 0 :
	    printf(" ");
	    break;
	case 1 :
	    switch(third) {
		case 0 :
		    printf("   B");
		    break;
		case 1 :
		    printf("   W");
		    break;
	    }
	    switch(second) {
		case 0 :
		    printf("P");
		    break;
		case 1 :
		    printf("Q");
		    break;
	    }
    }
}


/*
 * This function extracts the first element of the linked list
 * of moves and returns its value. It frees the element too.
 */
//struct move_seq *pop(struct move **list) {
//	// Adresse de la liste à libérér
//   	struct move *removed=*list;
//   	struct move_seq value;
//   	value = removed->move_seq;
//   	*list=removed->next;
//   	free(removed);
//   	return(value);
//}


/*
 * This function inserts the move move in
 * the list of moves of game.
 */
//int push(struct game *game, struct move move) {
//   struct move *t;
//   t = (struct node *)malloc(sizeof(struct move));
//   if(t==NULL) {
//      return -1;
//   }
//   else {
//      t->move_seq = move->move_seq;
//      t->next = game->moves;
//      game->moves = t;
//      return 0;
//   }
//}


/*
 * ===================================================================
 *                       Requested functions
 * ===================================================================
 */

void print_board(const struct game *game) {
    int i,j;
    for(i = 0; i<game->xsize; i++) {
      for(j = 0; j<game->ysize; j++) {
	    print_case(game->board[i][j]);
	    if(j == game->xsize-1) {
		// Better to create a function that takes xsize as an argument and produce
		// a line corresponding to the size of tabulation * xize (but it's a detail)
		printf("\n----------------------------------------------------------------------------------\n");
	    }
	    else {
        	printf("\t|");
	    }
	}
    }
}


//new_game

struct game *new_game(int xsize, int ysize) {
    struct game *new_state = (struct game *) malloc(sizeof (struct game));
    if(new_state == NULL) {
	return(NULL);
    }
    new_state->xsize=xsize;
    new_state->ysize=ysize;
    new_state->cur_player = PLAYER_WHITE;
    new_state->moves = (struct move *) malloc(sizeof(struct move));
    new_state->moves->seq = NULL;                                    //malloc ?
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

//struct game *load_game(int xsize, int ysize, const int **board, int cur_player) {
//    struct game *loaded_state = (struct game *) malloc(sizeof (struct game));
//    if(loaded_state == NULL) {
//	return(NULL);
//    }
//    loaded_state->xsize = xsize;
//    loaded_state->ysize = ysize;
//    loaded_state->cur_player = cur_player;
//    loaded_state->moves = (struct move *) malloc(sizeof(struct move));
//    loaded_state->moves->seq = NULL;                                    //malloc ?
//    loaded_state->board = (int **) malloc(ysize * sizeof(int *));
//    int i;
//    for (i = 0; i < ysize; i++) {
//	loaded_state->board[i] = (int *) malloc(xsize * sizeof(int));
//    }
//
//    int x,y;
//    for(x=0 ; x < xsize ; x++) {
//        for(y=0 ; y < ysize ; y++) {
//            loaded_game->board[x][y] = **board[x][y];   //This line must be checked :
//                                                        //as it's only a double pointer, I'm not sure of the syntax
//        }
//    }
//    return loaded_state;
//}

void free_game(struct game *game) {
    int i;
    for(i = 0 ; i < game->xsize ; i++) {
        free(game->board[i]);       //frees the memory used for the secondary tables in the primary table
    }
    free(game->board);              //frees the memory used for the primary table
    //  FREE MOVES TO DO
    free(game);                     //frees the memory used for the structure containing the game
}

//apply_move

/*
*To Do
*chgt en dame       DONE
*checkvictory
*current player     DONE
*push et pop        DONE
*/

/*
*This function applies a move list and inserts it in game->moves following this procedure :
* 1) initializing of the runners or other variables
* 2) checking the validity of a sequence
* 3) applying the move on the game
* 4) if a plaw has been taken :
*    5) updating the board
*    6) checking if the party is over (victory)
* 7) checking if the sequence has ended
* 8) if not : restart from 2) with the next element of the sequence
*    if yes :
*    9) checking the finale position of a plaw
*    10) transform the plaw into a queen if needed
*    11) changing the current player
*    12) inserting the move in game->moves
*    13) passing to the next element of the move list
*    14) if not the end of the move list : restart from 2)
*/
//int apply_moves(struct game *game, const struct move *moves) {
//    struct move *runner;                  //parcourt la liste de moves
//    struct move_seq *seq_runner;          //parcourt la sequence dans un move
//    runner = moves;
//    seq_runner = runner->seq;
//    struct move_seq *previous = game->moves->seq;   //mouvement precedent d'une sequence
//    previous = NULL;                                //initialement a NULL en debut de sequence
//
//    while(runner != NULL) {      //Tant que la liste de moves n'est pas vide
//
//        int validity;               //enregistre le resultat du test de validite
//        struct *coord taken;        //enregistre les coord d'une piece prise (potentiellement)
//
//
//        validity = is_move_seq_valid(game, seq_runner, previous, taken);
//
//        if(validity == 0)          //mouvement invalide
//            return (-1);
//        else {                     //mouvement valide
//            int oldx, oldy, newx, newy;
//            oldx = seq_runner->c_old.x;
//            oldy = seq_runner->c_old.y;
//            newx = seq_runner->c_new.x;
//            newy = seq_runner->c_new.y;
//            game->board[newx][newy] = game->board[oldx][oldy];   //reajuste la position de la piece depacee
//            game->board[oldx][oldy] = 0;
//
//            if(runner->seq->next == NULL) {  //si la liste de sequences dans
//                                             //l'element actuel de moves est terminee
//
//                if(game->cur_player == PLAYER_BLACK) {   //tour du jourur noir
//					if(newy == ysize-1 && game->board[newx][newy] == 1) {
//						game->board[newx][newy] = 3;     //pion noir -> dame noire
//					}
//					game->cur_player = PLAYER_WHITE;     //chgt joueur -> blanc
//				}
//                else {                                   //tour du joueur noir
//					if(newy == 0 && game->board[newx][newy] == 5) {
//						game->board[newx][newy] = 7;     //pion blanc -> dame blanche
//					}
//					game->cur_player = PLAYER_BLACK;     //chgt joueur -> noir
//				}
//
//				push(game, runner->seq);  //insere le move parcouru dans game->moves
//				runner = runner->next;    //passe a l'element suivant de moves
//				seq_runner = runner->seq; //ajuste le seq_runner sur runner
//				previous = NULL;          //reinitialise previous sur NULL (debut de sequence)
//
//            }
//
//            else {                          //si il reste des sequences dans
//                                            //l'element actuel de moves
//
//				previous = seq_runner,         //ajuste previous sur l'element analysé
//                seq_runner = seq_runner->next; //passe a l'element suivant de la sequence
//            }
//
//            if(validity == 2) {        //si prise
//
//                game->board[taken->x][taken->y] = 0;     //pliece prise devient vide
//                int victory = checkVictory(game);        //CREER UNE FONCTION CHECKVICTORY !!!
//                if(victory == 1)
//                    return (1);
//            }
//        }
//    }
//}


//is_seq_valid

/*
* verifie la validite d'une sequence suivant cette procedure :
* 1) c_old existe dans le plateau
* 2) il y a une piece en c_old
* 3) la piece en c_old appartient a cur_player
* 4) c_new existe dans le plateau
* 5) c_new est vide
* 6) deplacement diagonal
* 7) deux mouvements consecutifs sont des prises
* 8) une prise ne se fait que sur une piece adverse
* 9) si 2 prises consecutives : c_old(2e prise) = c_new(1re prise)
* si pion :
*   1) deplacement vers l'avant
*   2) deplacement de 1 case (sauf si prise)
*   3) si evolution en dame : dernier mouvement de move
* si dame :
*   1) se deplace d'autant que possible (jusqu'au bord ou une prise) en ligne droite
*   2) se deplace en avant OU en arriere
*/
int is_move_seq_valid(const struct game *game, const struct move_seq *seq, const struct move_seq *prev, struct coord *taken) {
    return(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct game *state = new_game(10,10);
    print_board(state);
    return(EXIT_SUCCESS);
}
