#include <stdlib.h>
#include <stdio.h>
#include "dames.h"
#include "aux.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#define BLACK_PAWN 1
#define WHITE_PAWN 5
#define BLACK_QUEEN 3
#define WHITE_QUEEN 7
#define EMPTY_CASE 0

/* Test Suite setup and cleanup functions: */

int init_suite(void) {return 0;}
int clean_suite(void) {return 0;}

/* Test case functions */

void test_new_game(void) {
    struct game *game = new_game(10,10);
    // NOTE : gérer le cas xsize ou ysize < 0 en retournant NULL ?
    CU_ASSERT_PTR_NOT_NULL(game);
    CU_ASSERT(game->xsize == 10);
    CU_ASSERT(game->ysize == 10);
    CU_ASSERT(game->cur_player == PLAYER_WHITE);
    CU_ASSERT_PTR_NULL(game->moves);
}

void test_load_game(void) {

}

void test_free_game(void) {
    // NOTE : comment tester cette fonction?
    struct game *game = new_game(10,10);
    free_game(game);
}

void test_push(void) {
    struct move *list = NULL;

    // On essaye d'ajouter un mouvement contenant une seule séquence
    struct coord c_old_1 = {1,2};
    struct coord c_new_1 = {3,4};
    struct move_seq *seq_1 = (struct move_seq *) malloc(sizeof(struct move_seq));
    seq_1->c_new = c_new_1;
    seq_1->c_old = c_old_1;
    seq_1->next = NULL;

    int push_1 = push(&list, seq_1);
    CU_ASSERT(push_1 == 0);
    // On vérifie que le première élément de la liste de move contient bien seq_1
    CU_ASSERT_PTR_EQUAL(list->seq, seq_1);
    // On vérifie que tous les informations de seq_1 sont correctes
    CU_ASSERT(list->seq->c_old.x == 1);
    CU_ASSERT(list->seq->c_old.y == 2);
    CU_ASSERT(list->seq->c_new.x == 3);
    CU_ASSERT(list->seq->c_new.y == 4);
    CU_ASSERT_PTR_NULL(list->seq->next);
    CU_ASSERT_PTR_NULL(list->next);

    // On va ajouter un deuxième mouvement, contenant cette fois deux séquences
    struct move_seq *seq_3 = (struct move_seq *) malloc(sizeof(struct move_seq));
    struct coord c_old_3 = {5,6};
    struct coord c_new_3 = {7,8};
    seq_3->c_new = c_new_3;
    seq_3->c_old = c_old_3;
    seq_3->next = NULL;

    struct coord c_old_2 = {1,2};
    struct coord c_new_2 = {3,4};
    struct move_seq *seq_2 = (struct move_seq *) malloc(sizeof(struct move_seq));
    seq_2->c_new = c_new_2;
    seq_2->c_old = c_old_2;
    seq_2->next = seq_3;

    int push_2 = push(&list, seq_2);
    CU_ASSERT(push_2 == 0);

    // On vérifie que toutes les séquences sont au bon endroit dans la liste
    CU_ASSERT_PTR_EQUAL(list->seq, seq_2);
    CU_ASSERT_PTR_EQUAL(list->seq->next, seq_3);
    CU_ASSERT_PTR_EQUAL(list->next->seq, seq_1);

    // Du coup, si les tests sur les pointeurs réussissent, les tests sur leur contenu
    // passeront aussi (et inversément), du coup je ne pense pas qu'il soit utile de tester
    // les contenus de chaque séquence après le push.
    free(seq_1);
    free(seq_2);
    free(seq_3);
}

void test_pop(void) {
    // Pour les tests sur pop, on va repartir de la liste de mouvement
    // construites pour les tests sur push.
    struct move *list = NULL;

    // On essaye d'ajouter un mouvement contenant une seule séquence
    struct coord c_old_1 = {1,2};
    struct coord c_new_1 = {3,4};
    struct move_seq *seq_1 = (struct move_seq *) malloc(sizeof(struct move_seq));
    seq_1->c_new = c_new_1;
    seq_1->c_old = c_old_1;
    seq_1->next = NULL;

    int push_1 = push(&list, seq_1);
    CU_ASSERT(push_1 == 0);

    // On va ajouter un deuxième mouvement, contenant cette fois deux séquences
    struct move_seq *seq_3 = (struct move_seq *) malloc(sizeof(struct move_seq));
    struct coord c_old_3 = {5,6};
    struct coord c_new_3 = {7,8};
    seq_3->c_new = c_new_3;
    seq_3->c_old = c_old_3;
    seq_3->next = NULL;

    struct coord c_old_2 = {1,2};
    struct coord c_new_2 = {3,4};
    struct move_seq *seq_2 = (struct move_seq *) malloc(sizeof(struct move_seq));
    seq_2->c_new = c_new_2;
    seq_2->c_old = c_old_2;
    seq_2->next = seq_3;

    int push_2 = push(&list, seq_2);
    CU_ASSERT(push_2 == 0);

    // On va maintenant popper les deux éléments de la liste de
    // mouvements et effectuer divers tests.
    struct move_seq *pop_1 = pop(&list);

    // On vérifie que l'élément retiré correspond bein au dernier ajouté,
    // c'est à dire seq_2
    CU_ASSERT_PTR_EQUAL(pop_1,seq_2);
    CU_ASSERT_PTR_EQUAL(pop_1->next, seq_3);
    // On vérifie bien que le sommet de la liste est bien de nouveau sur le
    // mouvement qui contient seq_1
    CU_ASSERT_PTR_EQUAL(list->seq, seq_1);

    struct move_seq *pop_2 = pop(&list);

    CU_ASSERT_PTR_EQUAL(pop_2,seq_1);
    CU_ASSERT_PTR_NULL(pop_2->next);
    CU_ASSERT_PTR_NULL(list);

    free(seq_1);
    free(seq_2);
    free(seq_3);
}

void test_is_move_seq_valid(void) {
    struct game *game = new_game(10,10);

    struct move_seq *seq = (struct move_seq *) malloc(sizeof(struct move_seq));
    struct coord *taken = (struct coord *) malloc(sizeof(struct coord));

    // Cas #1 : mouvement valide.
    struct coord c_old_1 = {3,6};
    struct coord c_new_1 = {4,5};
    seq->c_new = c_new_1;
    seq->c_old = c_old_1;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 1);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);
    // Cas #2 : mouvement non-valide de par sa longueur uniquement.
    struct coord c_old_2 = {3,6};
    struct coord c_new_2 = {5,4};
    seq->c_new = c_new_2;
    seq->c_old = c_old_2;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #3 : mouvement non-valide car vers une case claire (non-accessible)
    struct coord c_old_3 = {3,6};
    struct coord c_new_3 = {3,5};
    seq->c_new = c_new_3;
    seq->c_old = c_old_3;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #4 : mouvement non-valide car destination en dehors du plateau
    struct coord c_old_4 = {9,6};
    struct coord c_new_4 = {10,5};
    seq->c_new = c_new_4;
    seq->c_old = c_old_4;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #5 : mouvement non-valide car destination non-vide. Pour ce test, on modifie le plateau.
    game->board[5][4] = BLACK_PAWN;
    struct coord c_old_5 = {3,6};
    struct coord c_new_5 = {4,5};
    seq->c_new = c_new_5;
    seq->c_old = c_old_5;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #6 : mouvement non-valide car le pion n'appartient pas à cur_player
    struct coord c_old_6 = {2,3};
    struct coord c_new_6 = {1,4};
    seq->c_new = c_new_6;
    seq->c_old = c_old_6;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #7 : mouvement non-valide car il n'y a pas de pièce à la case de départ
    struct coord c_old_7 = {1,4};
    struct coord c_new_7 = {0,5};
    seq->c_new = c_new_7;
    seq->c_old = c_old_7;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #8 : mouvement non-valide car c_old est en dehors du plateau
    struct coord c_old_8 = {-2,5};
    struct coord c_new_8 = {0,5};
    seq->c_new = c_new_8;
    seq->c_old = c_old_8;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #9 : mouvement non-valide car c_old est inaccessible (case claire)
    struct coord c_old_9 = {2,4};
    struct coord c_new_9 = {0,5};
    seq->c_new = c_new_9;
    seq->c_old = c_old_9;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 0);
    CU_ASSERT(taken->x == -1);
    CU_ASSERT(taken->y == -1);

    // Cas #10 : mouvement valide d'un blanc vers l'avant/droite avec prise d'un pion
    print_board(game);
    struct coord c_old_10 = {3,6};
    struct coord c_new_10 = {5,4};
    seq->c_new = c_new_10;
    seq->c_old = c_old_10;

    CU_ASSERT(is_move_seq_valid(game, seq, NULL, taken) == 2);
    CU_ASSERT(taken->x == 4);
    CU_ASSERT(taken->y == 5);

    free(seq);
    free(taken);
}

void test_apply_moves(void)
{

}

void test_undo_moves(void) {

}

/* Test Runner Code goes here */

int main(void) {
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if(CUE_SUCCESS != CU_initialize_registry()) {
        return(CU_get_error());
    }

    /* add a suite to the registry */
    pSuite = CU_add_suite( "complete_test_suite", init_suite, clean_suite );
    if (pSuite == NULL) {
        CU_cleanup_registry();
        return(CU_get_error());
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test_new_game", test_new_game)) ||
        (NULL == CU_add_test(pSuite, "test_load_game", test_load_game)) ||
        (NULL == CU_add_test(pSuite, "test_free_game", test_free_game)) ||
        (NULL == CU_add_test(pSuite, "test_push", test_push)) ||
        (NULL == CU_add_test(pSuite, "test_pop", test_pop)) ||
        (NULL == CU_add_test(pSuite, "test_is_move_seq_valid", test_is_move_seq_valid))  ||
        (NULL == CU_add_test(pSuite, "test_apply_moves", test_apply_moves)) ||
        (NULL == CU_add_test(pSuite, "test_undo_moves", test_undo_moves))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_run_tests();
    return CU_get_error();
}
