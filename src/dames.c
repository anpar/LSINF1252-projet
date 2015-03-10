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
            if((game->board[x][y] == v1) || (game->board[x][y] == v2)) {
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
            print_case(game->board[x][y]);
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
    game->board = (int **) malloc(xsize * sizeof(int *));
    int i;
    for (i = 0; i < xsize; i++) {
        game->board[i] = (int *) malloc(ysize * sizeof(int));
    }

    // Remplissage du plateau
    int x,y;
    for (x = 0; x < xsize; x++) {
    	for (y = 0; y < ysize; y++) {
    	    // Cases vides
    	    if ((x+y) % 2 == 0)  {
                game->board[x][y] = EMPTY_CASE;
            }
            // Pions noirs
            else if (y < ysize/2 - 1) {
                game->board[x][y] = BLACK_PAWN;
            }
            // Pions blancs
            else if (y > ysize/2) {
                game->board[x][y] = WHITE_PAWN;
            }
            // Cases vides restantes
            else {
                game->board[x][y] = EMPTY_CASE;
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
    loaded_state->board = (int **) malloc(xsize * sizeof(int *));
    int i;
    for (i = 0; i < xsize; i++) {
        loaded_state->board[i] = (int *) malloc(ysize * sizeof(int));
    }

    // Remplisage du plateau
    int x,y;
    for(x=0 ; x < xsize ; x++) {
        for(y=0 ; y < ysize ; y++) {
            loaded_state->board[x][y] = board[x][y];
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
    for(i = 0; i < game->xsize; i++) {
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
    struct move *runner = (struct move *) malloc(sizeof(struct move));
    *runner = *moves;
    // parcourt la sequence dans un move
    struct move_seq *seq_runner = (struct move_seq *) malloc(sizeof(struct move_seq));
    if(runner != NULL) {
        *seq_runner = *runner->seq;;
    }
    runner->seq = seq_runner;
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
            seq_runner->piece_value = game->board[taken->x][taken->y];
            seq_runner->piece_taken = *taken;
            game->board[taken->x][taken->y] = EMPTY_CASE;
            // On vérifie que la prise de la pièce n'a pas entrainé la victoire d'un joueur
            if(check_victory(game) == 1) {
                return(1);
            }
        }
        // S'il n'y a pas eu de prise mais que le mouvement est valide
        else {
            seq_runner->piece_value = 0;
            seq_runner->piece_taken.x = -1;
            seq_runner->piece_taken.y = -1;
        }

        // Dans tous les cas, on doit quand même upate old_orig
        int newx = seq_runner->c_new.x;
        int newy = seq_runner->c_new.y;
        int oldx = seq_runner->c_old.x;
        int oldy = seq_runner->c_old.y;

        seq_runner->old_orig = game->board[oldx][oldy];

        // Et modifier le plateau en conséquence
        game->board[newx][newy] = game->board[oldx][oldy];
        game->board[oldx][oldy] = EMPTY_CASE;

        // Si on est à la dernière séquence du mouvement
        if(seq_runner->next == NULL) {
            // Si c'était le tour du joueur noir
            if(game->cur_player == PLAYER_BLACK) {
                // Si un pion noir a touché le bord
                if(newy == game->ysize-1 && game->board[newx][newy] == BLACK_PAWN) {
                    // Alors on le transforme en dame
                    game->board[newx][newy] = BLACK_QUEEN;
                }
                // C'est ensuite au joueur suivant de jouer
                game->cur_player = PLAYER_WHITE;
            }
            // Si c'était le tour du joueur blanc
            else {
                // Si un pion blanc à toucher le bord
                if(newy == 0 && game->board[newx][newy] == WHITE_PAWN) {
                    // Alors on le transforme en dame
                    game->board[newx][newy] = WHITE_QUEEN;
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

    // NOTE : on décide de mettre taken à -1 par défaut
    taken->x = -1;
    taken->y = -1;

    // On vérifie que c_old est bien dans les limites du plateau
    if (x_old > game->xsize-1 || x_old < 0 || y_old > game->ysize-1 || y_old < 0) {
        return(0);
    }
    // On vérifie que c_old est une coordonnées accessibles (i.e. une case foncée)
    if ((x_old + y_old) % 2 == 0) {
        return(0);
    }

    // On vérifie que la pièce en c_old appartient bien au joueur à qui c'est le tour
    if((game->cur_player) != ((game->board[x_old][y_old] & (1 << 2)) >> 2)) {
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

        if(prev->piece_value == 0) {
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
    if((game->board[x_new][y_new] & (1 << 0)) >> 0 == 1) {
        return(0);
    }

    // On doit maintenant vérifier que le déplacement effectué est correct
    // Pour cela, on doit envisager les différents cas

    // Si c'est un pion
    if(((game->board[x_old][y_old] & (1 << 1)) >> 1) == 0) {
        // Si c'est un pion blanc
        if(game->cur_player == PLAYER_WHITE) {
            // Déplacement classique, sans prise et donc vers l'avant
            if(y_new-y_old == -1 && abs(x_new-x_old) == 1) {
                return(1);
            }
            // Déplacement double vers l'avant et vers la droite (prise)
            else if(y_new-y_old == -2 && x_new-x_old == 2) {

                // On vérifie qu'il y bien prise d'un pièce adverse
                if(((game->board[x_old+1][y_old-1]) == BLACK_PAWN) || (((game->board[x_old+1][y_old-1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche (prise)
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[x_old-1][y_old-1]) == BLACK_PAWN) || (((game->board[x_old-1][y_old-1]) == BLACK_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite (prise)
            else if(y_new-y_old == 2 && x_new-x_old == 2) {
                if(((game->board[x_old+1][y_old+1]) == BLACK_PAWN) || (((game->board[x_old+1][y_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche (prise)
            else if(y_new-y_old == 2 && x_new-x_old == -2) {
                if(((game->board[x_old-1][y_old+1]) == BLACK_PAWN) || (((game->board[x_old-1][y_old+1]) == BLACK_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
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
                if(((game->board[x_old+1][y_old+1]) == WHITE_PAWN) || (((game->board[x_old+1][y_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'avant et vers la gauche (prise)
            else if(y_new-y_old == 2 && x_new-x_old == -2) {
                if(((game->board[x_old-1][y_old+1]) == WHITE_PAWN) || (((game->board[x_old-1][y_old+1]) == WHITE_QUEEN))) {
                    taken->x = x_old-1;
                    taken->y = y_old+1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la droite (prise)
            else if(y_new-y_old == -2 && x_new-x_old == 2) {
                if(((game->board[x_old+1][y_old-1]) == WHITE_PAWN) || (((game->board[x_old+1][y_old-1]) == WHITE_QUEEN))) {
                    taken->x = x_old+1;
                    taken->y = y_old-1;
                    return(2);
                }
            }
            // Déplacement double vers l'arrière et vers la gauche (prise)
            else if(y_new-y_old == -2 && x_new-x_old == -2) {
                if(((game->board[x_old-1][y_old-1]) == WHITE_PAWN) || (((game->board[x_old-1][y_old-1]) == WHITE_QUEEN))) {
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
                    // Déplacement vers le haut et vers la droite
                    // J'ai déplacer ces conditions car on doit d'abord incrément x_cur et y_cur
                    // avant de tester la deuxième condition
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

                    if(game->board[x_cur][y_cur] == BLACK_PAWN || game->board[x_cur][y_cur] == BLACK_QUEEN) {
                        taken->x = x_cur;
                        taken->y = y_cur;
                        return(2);
                    }
                    // On vérifie que la dame ne passe au dessus d'un pion de son équipe
                    else if(game->board[x_cur][y_cur] == WHITE_PAWN || game->board[x_cur][y_cur] == WHITE_QUEEN) {
                        return(0);
                    }
                }
            }
        // Si c'est une dame noire
        else {
            while(x_cur != x_new && y_cur != y_new) {
                // Déplacement vers le haut et vers la droite
                if((x_new-x_old) > 0 && (y_new-y_old) > 0) {
                    x_cur++;
                    y_cur++;
                }
                // Déplacement vers le bas et vers la droite
                else if((x_new-x_old) > 0 && (y_new-y_old < 0)) {
                    x_cur++;
                    y_cur--;
                }
                // Déplacement vers le haut et vers la gauche
                else if((x_new-x_old) < 0 && (y_new-y_old) > 0) {
                    x_cur--;
                    y_cur++;
                }
                // Déplacement vers le bas et vers la gauche
                else {
                    x_cur--;
                    y_cur--;
                }

                if(game->board[x_cur][y_cur] == WHITE_PAWN || game->board[x_cur][y_cur] == WHITE_QUEEN) {
                    taken->x = x_cur;
                    taken->y = y_cur;
                    return(2);
                }
                // On vérifie que la dame ne passe pas au dessus d'un pion de son équipe
                else if(game->board[x_cur][y_cur] == BLACK_PAWN || game->board[x_cur][y_cur] == BLACK_QUEEN) {
                    return(0);
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
		
		//retire le dernier mouvement effectué
		struct move_seq *popped;
		popped = pop(&(game->moves));
		while(popped != NULL) {
		//mise a jour du tableau
			int oldx = popped->c_old.x;
        	int oldy = popped->c_old.y;
        	int newx = popped->c_new.x;
        	int newy = popped->c_new.y;
			game->board[oldx][oldy] = popped->old_orig;
        	game->board[newx][newy] = EMPTY_CASE;
			//mise a jour d'une piece potentiellement prise
			if((popped->piece_taken.x != -1) || (popped->piece_taken.y != -1))
			{
				game->board[popped->piece_taken.x][popped->piece_taken.y] = popped->piece_value;
			}
			
			popped = popped->next;
		}
		//mise a jour du joueur actuel
		if(game->cur_player == PLAYER_BLACK)
		{
			game->cur_player = PLAYER_WHITE;
		}
		else
		{
			game->cur_player = PLAYER_BLACK;
		}
		count++;
	}
	return(EXIT_SUCCESS);
}
