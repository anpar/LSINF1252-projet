#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

struct game *plateau;

void print_board(const struct game *game) {
   int i,j;
   for(i=0; i<game->xsize;i++) {
      for(j=0; j<game->ysize;j++) {
         if(j==game->xsize-1) {
            printf("y\n");
	 }
         else if(j==0) {
            printf("\tx\t");
         }
	 else {
            printf("x\t");
         }		
      }
   }
   printf("\n");
}

struct game *new_game(int xsize, int ysize) {
   struct game *newplate = (struct game *) malloc(sizeof (struct game));
   if(newplate == NULL) { 
      return NULL;
   }  
   newplate->xsize=xsize;
   newplate->ysize=ysize;
   return newplate;
}

int main(int argc, const char *argv[]) {
   plateau = new_game(10,10);
   print_board(plateau);
}


