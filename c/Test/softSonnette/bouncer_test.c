#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/common.h"

#include "../../production/softSonnette/bouncer.c"

static int set_up(void **s) {
    bouncer_new();
    bouncer_start();
	return 0;
}

static int tear_down(void **s)
{
    bouncer_stop();
    bouncer_free();
    return 0;
}

static void test_bouncer_setRecognizeFace_ko(void **s) 
{
    EmployeeID employeeID;
    employeeID.ID = 2;    

    expect_function_call(__wrap_proxyUISP_signalFaceNotAllowed);

    bouncer_setRecognizeFace(employeeID);

}

static void test_bouncer_setRecognizeFace_ok(void **s) 
{
    EmployeeID employeeID;
    employeeID.ID = 2;
    Time time = {.year = 2023, .month = 6, .day = 12, .dayOfWeek = MONDAY, .hour = 10, .minute = 6, .second = 0};

    expect_function_call(__wrap_clock_getCurrentTime);

    will_return(__wrap_clock_getCurrentTime, &time);

    bouncer_setRecognizeFace(employeeID);

}

static void test_bouncer_checkEmployeeAllow_ok(void **s) 
{
    EmployeeID employeeID;
    employeeID.ID = 1;

    Time currentTime = {.dayOfWeek = MONDAY, .hour = 10, .minute = 30};

    bool valeurAttendue = true;
    
    bool ret = checkEmployeeAllow(currentTime, employeeID);

    assert_int_equal(valeurAttendue, ret);
}

static void test_bouncer_checkEmployeeAllow_ko(void **s) 
{
    EmployeeID employeeID;
    employeeID.ID = 1;

    Time currentTime = {.dayOfWeek = SATURDAY, .hour = 8, .minute = 30};

    bool valeurAttendue = false;
    
    bool ret = checkEmployeeAllow(currentTime, employeeID);

    assert_int_equal(valeurAttendue, ret);
}



static void test_MachineEtatBouncer_notAllowed(void **state)
{
    Picture myPicture = {.filePath = "test.jpg"};
    Employee_t* myEmployeeList;
    EmployeeID employeeID = {.ID = 1};

    bouncer_askFaceRecognition(); //premier trigger

    expect_function_call(__wrap_cameraman_suspendStreaming);

    expect_function_call(__wrap_cameraman_takePicture);

    will_return(__wrap_cameraman_takePicture, &myPicture);

    expect_function_call(__wrap_employeeManager_getEmployeeList);

    will_return(__wrap_employeeManager_getEmployeeList, &myEmployeeList);

    expect_function_call(__wrap_recognitionAI_launchAI);

    expect_value(__wrap_recognitionAI_launchAI, picture, &myPicture);

    expect_value(__wrap_recognitionAI_launchAI, employeeList, myEmployeeList);

    bouncer_setRecognizeFace(employeeID); //deuxième trigger

    assert_int_equal(recognizedEmployee.ID, employeeID.ID);

    expect_function_call(__wrap_cameraman_resumeStreaming);

    expect_function_call(__wrap_proxyUISP_signalFaceNotAllowed);

}


/**
 * Suite de test pour le module.
 */
const struct CMUnitTest bouncer_tests[] = 
{
	cmocka_unit_test(test_bouncer_checkEmployeeAllow_ok),
    cmocka_unit_test(test_bouncer_checkEmployeeAllow_ko),
    cmocka_unit_test(test_bouncer_setRecognizeFace_ok),
    cmocka_unit_test(test_bouncer_setRecognizeFace_ko),
    cmocka_unit_test(test_MachineEtatBouncer_notAllowed),
};



/**
 * Lancement de la suite de tests pour le module.
 */
/*
int main() 
{
    return cmocka_run_group_tests_name("Test du module Bouncer", bouncer_tests, set_up, tear_down);
}
*/