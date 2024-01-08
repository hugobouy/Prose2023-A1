#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/common.h"

#include "../../production/softSonnette/cameraman.c"

static int set_up(void **s) {
    cameraman_new();
    cameraman_start();
	return 0;
}

static int tear_down(void **s)
{
    cameraman_stop();
    cameraman_free();
    return 0;
}

static void test_MachineEtatCameraman_webcamKO(void **s)
{
    Transition *transit;
    State myState = S_IDLE;
    MqMsg msg;
    cameraman_startStreaming(); //premier trigger
    cameraman_mqReceive(&msg);
    transit =&mySm[myState][msg.data.event];

    assert_int_equal(transit->action,A_CHOIX);
    assert_int_equal(transit->destinationState,S_CHOIX);

    expect_function_call(__wrap_uiss_updateCamState);

    expect_value(__wrap_uiss_updateCamState, state, false);

    cameraman_checkCameraConnected();

    assert_int_equal(cameraAlive, 0);

}

static void test_checkCameraConnectedKO(void **s)
{

    expect_function_call(__wrap_uiss_updateCamState);

    expect_value(__wrap_uiss_updateCamState, state, false);

    cameraman_checkCameraConnected();

    assert_int_equal(cameraAlive, 0);

}

static void test_checkCameraConnectedOK(void **s)
{

    expect_function_call(__wrap_uiss_updateCamState);

    expect_value(__wrap_uiss_updateCamState, state, true);

    cameraman_checkCameraConnected();

    assert_int_equal(cameraAlive, 1);

}

static void test_MachineEtatCameraman_webcamOK(void **s)
{
    cameraman_startStreaming(); //premier trigger

    expect_function_call(__wrap_uiss_updateCamState);

    expect_value(__wrap_uiss_updateCamState, state, true);

    assert_int_equal(cameraAlive, 1);

    expect_function_call(__wrap_streamer_streamEcran);

    assert_int_equal(alreadylive, 1);

    cameraman_subscribeToVideoStream(true); //deuxième trigger

    expect_function_call(__wrap_streamer_streamTel);

    cameraman_suspendStreaming(); //troisième trigger

    expect_function_call(__wrap_streamer_stopPipeline);

    expect_function_call(__wrap_streamer_cleanPipeline);

    assert_int_equal(alreadylive, 0);

}


/**
 * Suite de test pour le module.
 */
const struct CMUnitTest cameraman_tests[] = 
{
    cmocka_unit_test(test_MachineEtatCameraman_webcamKO),
    cmocka_unit_test(test_MachineEtatCameraman_webcamOK),
};



/**
 * Lancement de la suite de tests pour le module.
 */
int main() 
{
    return cmocka_run_group_tests_name("Test du module Cameraman", cameraman_tests, set_up, tear_down);
}