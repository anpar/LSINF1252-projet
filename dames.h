#ifndef _DAMES_H
#define _DAMES_H

#define PLAYER_BLACK    0
#define PLAYER_WHITE    1

/*
 * Structure du jeu.
 * @board représente le plateau de jeu. Chaque case est représentée par
 *         une valeur entière dont la signification est la suivante:
 *         [...|C|T|P] sont les 3 bits de poid faible de chaque valeur
 *         entière. Le bit C est le bit de couleur: 0 = noir, 1 = blanc.
 *         Le bit T est le bit de type de pièce: 0 = pion, 1 = dame.
 *         Le bit P est le bit de présence: 0 = case vide, 1 = case remplie.
 *         board[i][j] représente donc l'état de la case aux coordonnées (i,j).
 * @xsize et @ysize stockent la taille du jeu
 * @moves enregistre la liste des mouvements effectués
 * @cur_player enregistre le joueur actuel (i.e. qui doit effectuer le prochain mouvement)
 *             (PLAYER_BLACK ou PLAYER_WHITE)
 */
struct game {
    int **board;
    int xsize, ysize;

    struct move *moves;
    int cur_player;
};

/* Coordonnées */
struct coord {
    int x, y;
};

/*
 * Liste de la séquence d'un mouvement
 */
struct move_seq {
    struct move_seq *next;
    struct coord c_old;     /* coordonnées de départ */
    struct coord c_new;     /* coordonnées de destination */

    /*
     * Les champs suivants ne sont utilisés que lorsque l'élément fait partie
     * des mouvements effectués dans game->moves.
     *
     * @piece_value est la valeur entière d'une pièce capturée lors du mouvement.
     *              0 si pas de pièce capturée.
     * @piece_taken contient les coordonnées de la pièce capturée
     * @old_orig contient la valeur entière de la pièce se situant en @c_old avant
     *           l'exécution du mouvement.
     */
    int piece_value;
    struct coord piece_taken;
    int old_orig;
};

/*
 * Liste de mouvements
 */
struct move {
    struct move *next;
    struct move_seq *seq;
};

/*
 * new_game
 * Créer un nouveau jeu avec la position initiale
 *
 * @xsize: taille du jeu pour les abscisses (i.e. nombre de colonnes)
 * @ysize: taille du jeu pour les ordonnées (i.e. nombre de lignes)
 * @return: Pointeur vers la structure du jeu ou NULL si erreur
 */
extern struct game *new_game(int xsize, int ysize);

/*
 * load_game
 * Charger un jeu depuis une position donnée
 *
 * @xsize: taille du jeu pour les abscisses (i.e. nombre de colonnes)
 * @ysize: taille du jeu pour les ordonnées (i.e. nombre de lignes)
 * @board: tableau à deux dimensions représentant l'état du jeu
 * @cur_player: joueur actuel (PLAYER_WHITE ou PLAYER_BLACK)
 * @return Pointeur vers la structure du jeu ou NULL si erreur
 */
extern struct game *load_game(int xsize, int ysize, const int **board, int cur_player);

/*
 * free_game
 * Libérer les ressources allouées à un jeu
 *
 * @game: pointeur vers la structure du jeu
 */
extern void free_game(struct game *game);

/*
 * apply_moves
 * Effectuer des mouvements
 *
 * @game: pointeur vers la structure du jeu
 * @moves: liste chainée de mouvements à effectuer
 * @return: -1 si erreur (e.g. mouvement invalide), 0 si mouvements valides, 1 si jeu terminé (game->cur_player est le gagnant)
 */
extern int apply_moves(struct game *game, const struct move *moves);

/*
 * is_move_seq_valid
 * Vérifier si une séquence d'un mouvement est valide. La fonction ne MODIFIE PAS l'état du jeu !
 * 
 * @game: pointeur vers la structure du jeu
 * @seq: séquence du mouvement à vérifier
 * @prev: séquence précédent immédiatement la séquence à vérifier, NULL s'il @seq est la première séquence du mouvement
 * @taken: pointeur vers des coordonnées qui seront définies aux coordonnées de la pièce prise s'il en existe une
 * @return: 0 si mouvement invalide, 1 si déplacement uniquement, 2 si capture
 */
extern int is_move_seq_valid(const struct game *game, const struct move_seq *seq, const struct move_seq *prev, struct coord *taken);

/*
 * undo_moves
 * Annule des mouvements et remet l'état du jeu correspondant
 *
 * @game: pointeur vers la structure du jeu
 * @n: nombre de coups à annuler. Si n > nombre total de mouvements, alors ignorer le surplus.
 * @return: 0 si OK, -1 si erreur
 */
extern int undo_moves(struct game *game, int n);

/*
 * print_board
 * Affiche l'état du jeu sur le terminal
 *
 * @game: pointeur vers la structure du jeu
 */
extern void print_board(const struct game *game);

#endif
