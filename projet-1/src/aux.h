#ifndef _AUX_H
#define _AUX_H

/*
    Header contenant les signatures des fonctions
    auxiliaires utilisées dans dames.c. Permet de
    tester aussi les fonctions auxiliaires dans test.c.
*/

extern void print_case(int board_case);
struct move_seq *pop(struct move **list);
int push(struct move **list, struct move_seq *value);
int checkVictory(struct game *game);
extern void reverse(struct move_seq **list);

#endif
