#include <stdlib.h>
#include <stdio.h>
#include "dames.h"
#include "aux.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

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
    struct move_seq *seq_1;
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
    struct move_seq *seq_3;
    struct coord c_old_3 = {5,6};
    struct coord c_new_3 = {7,8};
    seq_3->c_new = c_new_3;
    seq_3->c_old = c_old_3;
    seq_3->next = NULL;

    struct coord c_old_2 = {1,2};
    struct coord c_new_2 = {3,4};
    struct move_seq *seq_2;
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
}


void test_pop(void) {
    // Pour les tests sur pop, on va repartir de la liste de mouvement
    // construites pour les tests sur push.
    struct move *list = NULL;

    // On essaye d'ajouter un mouvement contenant une seule séquence
    struct coord c_old_1 = {1,2};
    struct coord c_new_1 = {3,4};
    struct move_seq *seq_1;
    seq_1->c_new = c_new_1;
    seq_1->c_old = c_old_1;
    seq_1->next = NULL;

    int push_1 = push(&list, seq_1);
    CU_ASSERT(push_1 == 0);

    // On va ajouter un deuxième mouvement, contenant cette fois deux séquences
    struct move_seq *seq_3;
    struct coord c_old_3 = {5,6};
    struct coord c_new_3 = {7,8};
    seq_3->c_new = c_new_3;
    seq_3->c_old = c_old_3;
    seq_3->next = NULL;

    struct coord c_old_2 = {1,2};
    struct coord c_new_2 = {3,4};
    struct move_seq *seq_2;
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
}


void test_is_move_seq_valid(void) {
    struct game *game = new_game(10,10);

    // Cas #1 : mouvement valide.
    struct coord c_old_1 = {3,6};
    struct coord c_new_1 = {4,5};
    struct move_seq *seq_1;
    seq_1->c_new = c_new_1;
    seq_1->c_old = c_old_1;
    struct coord *taken_1;

    CU_ASSERT(is_move_seq_valid(game, seq_1, NULL, taken_1) == 1);
    CU_ASSERT_PTR_NULL(taken_1);

    // Cas #2 : mouvement non-valide de par sa longueur uniquement.
    struct coord c_old_2 = {3,6};
    struct coord c_new_2 = {5,4};
    struct move_seq *seq_2;
    seq_2->c_new = c_new_2;
    seq_2->c_old = c_old_2;
    struct coord *taken_2;

    CU_ASSERT(is_move_seq_valid(game, seq_2, NULL, taken_2) == 0);
    CU_ASSERT_PTR_NULL(taken_2);

    // Cas #3 : mouvement non-valide car vers une case claire (non-accessible)
    struct coord c_old_3 = {3,6};
    struct coord c_new_3 = {5,4};
    struct move_seq *seq_3;
    seq_3->c_new = c_new_3;
    seq_3->c_old = c_old_3;
    struct coord *taken_3;

    CU_ASSERT(is_move_seq_valid(game, seq_3, NULL, taken_3) == 0);
    CU_ASSERT_PTR_NULL(taken_3);
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
