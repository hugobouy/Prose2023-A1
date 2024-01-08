#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/guard.c"
#include "../../production/softSonnette/proxyConnectionManager.h"


/**
 * @brief permet de choisir quel test est en cours pour vérifier l'argument donné dans l'appel de la fonction proxyConnectionManager_validatePass
 * 
 */
typedef enum
{
    TEST_MDP_INVALIDE = 0, TEST_MDP_VALIDE
}e_testMDP;
e_testMDP testEnCours;

/**
 * @brief créé un mock de la fonction proxyConnectionManager_validatePass
 * 
 * @param passValidated 
 */
void __wrap_proxyConnectionManager_validatePass(bool passValidated);
 
void __wrap_proxyConnectionManager_validatePass(bool passValidated)
{
    //la fonction a été appelée par guard_askCheckPass
    function_called();

    //test de la valeur du paramètre en fonction du mot de passe envoyé
    if (testEnCours == TEST_MDP_VALIDE)
    assert_true(passValidated);
    if (testEnCours == TEST_MDP_INVALIDE)
    assert_false(passValidated);
}

void test_motDePasseValide(void **)
{
    testEnCours = TEST_MDP_VALIDE;
    expect_function_call(__wrap_proxyConnectionManager_validatePass);
    guard_askCheckPass("1234");
}

void test_motDePasseInvalide(void **)
{
    testEnCours = TEST_MDP_INVALIDE;
    //test de tous les mots de passe incorrects possible
    for (int i=0; i<=9999; i++)
    {
        if (i!= 1234)
        {
            expect_function_call(__wrap_proxyConnectionManager_validatePass);
            guard_askCheckPass("4244");
        }
    }
}



/*
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_motDePasseValide),
        cmocka_unit_test(test_motDePasseInvalide),

    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
*/
