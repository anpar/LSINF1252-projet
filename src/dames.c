#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

// Variable globale permettant de stocker l'état du jeu.
struct game *state;
  

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
    // Here is a link that explain how to extract the kth bit from
    // a byte : http://stackoverflow.com/questions/2249731/how-to-get-bit-by-bit-data-from-a-integer-value-in-c
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


//load_game

struct game *load_game(int xsize, int ysize, const int **board, int cur_player) {
    cur_black = 0;  //initialise le nombre de pieces
    cur_white = 0;
    struct game *loaded_state = (struct game *) malloc(sizeof (struct game));
    if(loaded_state == NULL) { 
	return(NULL);
    } 
    loaded_state->xsize = xsize;
    loaded_state->ysize = ysize;
    loaded_state->cur_player = cur_player;
    loaded_state->board = (int **) malloc(ysize * sizeof(int *));
    int i;
    for (i = 0; i < ysize; i++) {
	loaded_state->board[i] = (int *) malloc(xsize * sizeof(int));
    }

    int x,y;
    for(x=0 ; x < xsize ; x++) {
        for(y=0 ; y < ysize ; y++) {
            loaded_game->board[x][y] = **board[x][y];   //This line must be checked : 
                                                        //as it's only a double pointer, I'm not sure of the syntax
        }
    }
    return loaded_state;
}


//free_game

void free_game(struct game *game) {
    int i;
    for(i = 0 ; i < game->xsize ; i++) {
        free(game->board[i]);       //frees the memory used for the secondary tables in the primary table
    }
    free(game->board);              //frees the memory used for the primary table
    free(game);                     //frees the memory used for the structure containing the game
}


//apply_move

int apply_moves(struct game *game, const struct move *moves) {
    /*int foe;   //determine la quantite de pieces du joueur attaque en cas de perte de ce dernier
    if(game->cur_player==PLAYER_WHITE) {
        foe = cur_black;
    }
    else {foe = cur_white;}

    while(moves != NULL) {
        if(foe==0)         //si cur_player a gagne
            return(1);
        if(moves->
    }*/
}


int main(int argc, const char *argv[]) {
    state = new_game(10,10);
    print_board(state);
    return(EXIT_SUCCESS);
}
