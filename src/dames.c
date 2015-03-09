#include <stdlib.h>
#include <stdio.h>
#include "dames.h"

#define BLACK_PAWN 1
#define WHITE_PAWN 5
#define BLACK_QUEEN 3
#define WHITE_QUEEN 7
#define EMPTY_CASE 0

/*
    ===================================
    FONCTIONS AUXILIAIRES
    ===================================
*/

/*
    print_case affiche la chaine de caractère
    correspondant au byte représentant une case.
    Les lettres B et W représentent la couleur (Black
    ou White) tandis que les lettres P et Q représentent
    le type de pièce (Pawn ou Queen). Quand la case
    est vide, elle affiche un espace.
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
    pop supprime le premier élement d'une
    liste de mouvement et retourne la
    séquence de mouvement associé.
*/
struct move_seq *pop(struct move **list) {
    if(list == NULL) {
        return(NULL);
    }

    struct move_seq *popped = (*list)->seq;
	struct move *removed_move = *list;
	*list = (*list)->next;
	free(removed_move);

	return(popped);
}

/*
    push ajoute un mouvement dans la liste
    de mouvement. On l'utilise donc
    une fois que le mouvement a été appliqué
    par apply_moves. Elle retourne 0 si tout
    s'est bien passé, -1 sinon (pas assez de
    mémoire par en allouer).
*/
int push(struct move **list, struct move_seq *value) {
    struct move *temp = (struct move *) malloc(sizeof(struct move));
    if(temp == NULL) {
        return(-1);
    }

	temp->seq = value;
	temp->next = *list;
	*list = temp;
	return(0);
}

/*
    check_victory vérifie si un des deux joueurs
    a gagné ou non. Elle retourne 1 si un des joueurs
    a gagné, 0 sinon.
*/
int check_victory(struct game *game) {
    int x,y,v1,v2;

    if(game->cur_player == PLAYER_BLACK) {
        v1 = WHITE_PAWN;
        v2 = WHITE_QUEEN;
    }
    else {
        v1 = BLACK_PAWN;
        v2 = BLACK_QUEEN;
    }

    for(x = 0 ; x < game->xsize; x++) {
        for(y = 0 ; y < game->ysize; y++) {
            // On vérifie s'il reste des pièces de types v1 ou v2
            if((game->board[y][x] == v1) || (game->board[y][x] == v2)) {
                // Si c'est le cas, il reste des pièces adverses
                return(0);
            }
        }
    }
    // Sinon, c'est qu'il ne reste plus de pièce adverse
    return(1);
}

/*
    ===================================
    FONCTIONS OBLIGATOIRES
    ===================================
*/

/*
    print_board
*/
void print_board(const struct game *game) {
    int x,y;
    for(y = 0; y < game->ysize; y++) {
        for(x = 0; x < game->xsize; x++) {
            print_case(game->board[y][x]);
            if(x == game->xsize-1) {
                // TODO : faire une fonction qui génère une ligne qui dépend de xsize
                printf("\n----------------------------------------------------------------------------------\n");
            }
            else {
                printf(" \t|");
            }
        }
    }
}

/*
    new_game
*/
struct game *new_game(int xsize, int ysize) {
    struct game *game = (struct game *) malloc(sizeof (struct game));
    if(game == NULL) {
        return(NULL);
    }

    // Initialisations triviales
    game->xsize=xsize;
    game->ysize=ysize;
    game->cur_player = PLAYER_WHITE;
    game->moves = NULL;

    // Allocations dynamiques de board
    game->board = (int **) malloc(ysize * sizeof(int *));
    int i;
    for (i = 0; i < ysize; i++) {
        game->board[i] = (int *) malloc(xsize * sizeof(int));
    }

    // Remplissage du plateau
    int x,y;
    for (x = 0; x < xsize; x++) {
    	for (y = 0; y < ysize; y++) {
    	    // Cases vides
    	    if ((x+y) % 2 == 0)  {
                game->board[y][x] = EMPTY_CASE;
            }
            // Pions noirs
            else if (y < 4) {
                game->board[y][x] = BLACK_PAWN;
            }
            // Pions blancs
            else if (y > ysize - 5) {
                game->board[y][x] = WHITE_PAWN;
            }
            // Cases vides restantes
            else {
                game->board[y][x] = EMPTY_CASE;
            }
    	}
    }
    return(game);
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

    // Initilisations triviales
    loaded_state->xsize = xsize;
    loaded_state->ysize = ysize;
    loaded_state->cur_player = cur_player;
    loaded_state->moves = NULL;

    // Allocations dynamiques de board
    loaded_state->board = (int **) malloc(ysize * sizeof(int *));
    int i;
    for (i = 0; i < ysize; i++) {
        loaded_state->board[i] = (int *) malloc(xsize * sizeof(int));
    }

    // Remplisage du plateau
    int x,y;
    for(x=0 ; x < xsize ; x++) {
        for(y=0 ; y < ysize ; y++) {
            loaded_state->board[y][x] = board[y][x];
        }
    }

    return(loaded_state);
}

/*
    free_game
*/
void free_game(struct game *game) {
    // On libère d'abord le plateau
    int i;
    for(i = 0; i < game->ysize; i++) {
        free(game->board[i]);
    }
    free(game->board);

    // On libère maintenant la liste de mouvement
    while(game->moves != NULL) {
		pop(&(game->moves));
	}

    free(game);
}

/*
    apply_moves
*/

int apply_moves(struct game *game, const struct move *moves) {
    // parcourt la liste de moves (avec un typecast pour supprimer une warning à cause du const)
    struct move *runner = (struct move *) moves;
    // parcourt la sequence dans un move
    struct move_seq *seq_runner;
    if(runner != NULL) {
        seq_runner = runner->seq;
    }
    // mouvement precedent d'une sequence, NULL initialement
    struct move_seq *previous = NULL;

    // Tant que la liste de moves n'est pas vide
    while(runner != NULL) {
        struct coord *taken = (struct coord *) malloc(sizeof(struct coord));
        // On vérifie que l'allocation a bien fonctionné
        if(taken == NULL) {
            return(-1);
        }

        int validity = is_move_seq_valid(game, seq_runner, previous, taken);

        // Si le mouvement est invalide
        if(validity == 0) {
            return(-1);
        }
        // S'il y a eu une prise
        else if(validity == 2) {
            seq_runner->piece_value = game->board[taken->y][taken->x];
            seq_runner->piece_taken = *taken;
            game->board[taken->y][taken->x] = EMPTY_CASE;
            // On vérifie que la prise de la pièce n'a pas entrainé la victoire d'un joueur
            if(check_victory(game) == 1) {
                return(1);
            }
        }
        // S'il n'y a pas eu de prise mais que le mouvement est valide
        else {
            seq_runner->piece_value = 0;
            // NOTE : par défaut (0;0), il faudra quand même vérifier que ça fonction bien
            seq_runner->piece_taken.x = 0;
            seq_runner->piece_taken.y = 0;
        }

        // Dans tous les cas, on doit quand même upate old_orig
        int newx = seq_runner->c_new.x;
        int newy = seq_runner->c_new.y;
        int oldx = seq_runner->c_old.x;
        int oldy = seq_runner->c_old.y;

        seq_runner->old_orig = game->board[oldy][oldx];

        // Et modifier le plateau en conséquence
        game->board[newy][newx] = game->board[oldy][oldx];
        game->board[oldy][oldx] = EMPTY_CASE;

        // Si on est à la dernière séquence du mouvement
        if(seq_runner->next == NULL) {
            // Si c'était le tour du joueur noir
            if(game->cur_player == PLAYER_BLACK) {
                // Si un pion noir a touché le bord
                if(newy == game->ysize-1 && game->board[newy][newx] == BLACK_PAWN) {
                    // Alors on le transforme en dame
                    game->board[newy][newx] = BLACK_QUEEN;
                }
                // C'est ensuite au joueur suivant de jouer
                game->cur_player = PLAYER_WHITE;
            }
            // Si c'était le tour du joueur blanc
            else {
                // Si un pion blanc à toucher le bord
                if(newy == 0 && game->board[newy][newx] == WHITE_PAWN) {
                    // Alors on le transforme en dame
                    game->board[newy][newx] = WHITE_QUEEN;
                }
                // C'est ensuite au joueur suivant de jouer
                game->cur_player = PLAYER_BLACK;
            }

            // Il faut ajouter le mouvement ainsi terminé dans game->moves
            push(&(game->moves), runner->seq);
            // Et on passe ensuite au mouvement suivant
            runner = runner->next;
            if(runner != NULL) {
                seq_runner = runner->seq;
            }
            previous = NULL;
        }
        // Si il reste des sequences dans l'element actuel de moves
        else {
            // On ajuste previous sur l'element précédent
            previous = seq_runner;
            // On passe a l'element suivant de la sequence
            seq_runner = seq_runner->next;
        }
        free(taken);
        taken = NULL;
    }
	return(0);
}

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
		struct move_seq *popped;
		popped = pop(&(game->moves));
		while(popped != NULL) {
			int oldx = popped->c_old.x;
        	int oldy = popped->c_old.y;
        	int newx = popped->c_new.x;
        	int newy = popped->c_new.y;
			game->board[oldy][oldx] = popped->old_orig;
        	game->board[newy][newx] = EMPTY_CASE;
			game->board[popped->piece_taken.y][popped->piece_taken.x] = popped->piece_value;
			popped = popped->next;
		}
	}
	return(EXIT_SUCCESS);
}

/*
    Je vire le main car sinon on ne peut pas tester programme.c et test.c.
*/

//int main(int argc, char *argv[]) {
	//PREMIERS TESTS EFFECTUES

    /*struct game *state = new_game(10,10);
	state->moves = NULL;

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


    struct coord c_old4 = {5,4};
    struct coord c_new4= {3,6};
    struct move_seq move_seq4;
    move_seq4.c_new = c_new4;
    move_seq4.c_old = c_old4;
    move_seq4.next = NULL;
    struct move mouvement4 = {NULL, &move_seq4};


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


    struct coord c_old6 = {5,4};
    struct coord c_new6= {3,6};
    struct move_seq move_seq6;
    move_seq6.c_new = c_new6;
    move_seq6.c_old = c_old6;
    move_seq6.next = NULL;
    struct move mouvement6 = {NULL, &move_seq6};


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

	printf("TOUS LES TESTS SE SONT TERMINES AVEC SUCCES\n");*/


    //second tests

//    struct game *state = new_game(10,10);
//    printf("C'est au tour du joueur %d.\n", state->cur_player);
//    print_board(state);
//    printf("\n\n\n");
//
//    /*
//        Mouvement 4
//    */
//    struct coord c_old4 = {5,4};
//    struct coord c_new4= {3,6};
//    struct move_seq move_seq4;
//    move_seq4.c_new = c_new4;
//    move_seq4.c_old = c_old4;
//    move_seq4.next = NULL;
//    struct move mouvement4 = {NULL, &move_seq4};
//
//    /*
//        Mouvement 3
//    */
//    struct coord c_old3 = {5,6};
//    struct coord c_new3= {6,5};
//    struct move_seq move_seq3;
//    move_seq3.c_new = c_new3;
//    move_seq3.c_old = c_old3;
//    move_seq3.next = NULL;
//    struct move mouvement3 = {&mouvement4, &move_seq3};
//
//    /*
//        Mouvement 2
//    */
//    struct coord c_old2 = {4,3};
//    struct coord c_new2= {5,4};
//    struct move_seq move_seq2;
//    move_seq2.c_new = c_new2;
//    move_seq2.c_old = c_old2;
//    move_seq2.next = NULL;
//    struct move mouvement2 = {&mouvement3, &move_seq2};
//
//    /*
//        Mouvement 1
//    */
//    struct coord c_old1 = {3,6};
//    struct coord c_new1= {4,5};
//    struct move_seq move_seq1;
//    move_seq1.c_new = c_new1;
//    move_seq1.c_old = c_old1;
//    move_seq1.next = NULL;
//    struct move mouvement1 = {&mouvement2, &move_seq1};
//    int apply_moves_result1 = apply_moves(state, &mouvement1);
//    print_board(state);
//    printf("\n");
//
//    /*
//        Affichons game->move pour voir ce que ça dit
//    */
//    printf("Adresse du premier mouvement : %p.\n", state->moves);
//    printf("Adresse de la première séquence du premier mouvement : %p.\n", state->moves->seq);
//    printf("Déplacement de (%d,%d) à (%d,%d).\n", state->moves->seq->c_old.x, state->moves->seq->c_old.y, state->moves->seq->c_new.x, state->moves->seq->c_new.y);
//    printf("Adresse de la deuxième séquence du premier mouvement : %p.\n", state->moves->seq->next);
//
//    printf("tests sur la prise\n");
//    printf("adresse de moves->next : %p.\n",state->moves->next);
//	printf("adresse de moves->next->move_seq : %p.\n",state->moves->next->seq);
//	printf("contenu de moves->next->move_seq :\nc_old : (%d,%d)\nc_new : (%d,%d)\n\n",state->moves->next->seq->c_old.x,state->moves->next->seq->c_old.y,state->moves->next->seq->c_new.x,state->moves->next->seq->c_new.y);
//    printf("contenu de taken : (%d,%d).\n",state->moves->seq->piece_taken.x,state->moves->seq->piece_taken.y);
//    printf("contenu old_orig : %d.\n", state->moves->seq->old_orig);
//    printf("contenu de piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->seq->piece_value);
//    printf("tests de prise precedente\n");
//    printf("contenu de next->taken : (%d,%d).\n",state->moves->next->seq->piece_taken.x,state->moves->next->seq->piece_taken.y);
//    printf("contenu next->old_orig : %d.\n", state->moves->next->seq->old_orig);
//    printf("contenu de next->piece_value (vu les tests : peut etre n'importe quoi) : %d.\n\n", state->moves->next->seq->piece_value);
//
//    printf("test pop\n");
//    struct move_seq *pop1 = pop(&(state->moves));
//    printf("Adresse de l'avant-dernier mouvement : %p.\n", state->moves);
//
//    // NOTE : la dernière ligne retourne (nil). ça prouve selon moi qu'il y a un problème avec
//    // les fonctions push/pop.
//
//    free_game(state);
//}
