#include <stdlib.h>
#include <stdio.h>
#include "dames.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

/* Test Suite setup and cleanup functions: */

int init_suite(void) {return 0;}
int clean_suite(void) {return 0;}

/* Test case functions */

void test_new_game(void) {

}

void test_load_game(void) {

}

void test_free_game(void) {

}

void test_is_move_seq_valid(void) {

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
        (NULL == CU_add_test(pSuite, "test_is_move_seq_valid", test_is_move_seq_valid))  ||
        (NULL == CU_add_test(pSuite, "test_apply_moves", test_apply_moves)) ||
        (NULL == CU_add_test(pSuite, "test_undo_moves", test_undo_moves))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_SILENT);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
