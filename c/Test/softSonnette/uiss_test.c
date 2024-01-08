
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
//#include "pthread.h"

#include "../../production/softSonnette/common.h"
// extern pthread_barrier_t start_barrier;
// extern pthread_barrier_t stop_barrier;
// extern pthread_barrier_t quit_barrier;
// extern pthread_mutex_t employee_mutex;

#include "../../production/softSonnette/uiss.c"

static int set_up(void **state) 
{  
    uiss_new();
    uiss_start();
	return 0;
}

static int tear_down(void **state) 
{
    uiss_stop();
    uiss_free();
	return 0;
}


static void test_MachineEtatUiss_transition_1_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_INIT;
  MqMsg msg = {.data.event = E_ASK_START_COM} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_ACK_START_COM);
  assert_int_equal(transit->destinationState,S_IDLE);
}

static void test_MachineEtatUiss_transition_1_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_INIT;
  MqMsg msg; //= {.data.event = E_ASK_START_COM} ;
  uiss_askStartCom();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_ACK_START_COM);
  assert_int_equal(transit->destinationState,S_IDLE);
}

static void test_MachineEtatUiss_transition_2_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_INIT;
  MqMsg msg = {.data.event = E_TIMEOUT_TASS} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_ERROR_COM);
  assert_int_equal(transit->destinationState,S_ERROR_COM);
}

// static void test_MachineEtatUiss_transition_2_avec_mq(void **state)
// {
//   Transition *transit;
//   State myState = S_INIT;
//   MqMsg msg;
//   uiss_timeout_tass(&uiss_timeout_tass);
//   uiss_mqReceive(&msg);
//   transit =&mySm[myState][msg.data.event];

//   assert_int_equal(transit->action,A_ERROR_COM);
//   assert_int_equal(transit->destinationState,S_ERROR_COM);
// }

static void test_MachineEtatUiss_transition_3_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg = {.data.event = E_UPDATE_CAM_TRUE} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_CAM_TRUE);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_3_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg;
  uiss_updateCamState(true);
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_CAM_TRUE);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_4_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg = {.data.event = E_UPDATE_CAM_FALSE} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_CAM_FALSE);
  assert_int_equal(transit->destinationState,S_ERROR_CAM_NOT_CONNECTED);
}

static void test_MachineEtatUiss_transition_4_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg;
  uiss_updateCamState(false);
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_CAM_FALSE);
  assert_int_equal(transit->destinationState,S_ERROR_CAM_NOT_CONNECTED);
}

static void test_MachineEtatUiss_transition_5_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg = {.data.event = E_RING} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_RING);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_5_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg;
  uiss_ring();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_RING);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_6_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg = {.data.event = E_UPDATE_DOOR} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_DOOR);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_6_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg;
  uiss_updateDoorState(true);
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_UPDATE_DOOR);
  assert_int_equal(transit->destinationState,S_RUNNING);
}

static void test_MachineEtatUiss_transition_7_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg = {.data.event = E_STOP} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_7_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_RUNNING;
  MqMsg msg;
  uiss_stop();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_8_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg = {.data.event = E_STOP} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_8_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_IDLE;
  MqMsg msg;
  uiss_stop();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_9_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_INIT;
  MqMsg msg = {.data.event = E_STOP} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_9_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_INIT;
  MqMsg msg ;
  uiss_stop();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_10_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_ERROR_COM;
  MqMsg msg = {.data.event = E_STOP} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_10_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_ERROR_COM;
  MqMsg msg ;
  uiss_stop();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_11_sans_mq(void **state)
{
  Transition *transit;
  State myState = S_ERROR_CAM_NOT_CONNECTED;
  MqMsg msg = {.data.event = E_STOP} ;
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

static void test_MachineEtatUiss_transition_11_avec_mq(void **state)
{
  Transition *transit;
  State myState = S_ERROR_CAM_NOT_CONNECTED;
  MqMsg msg = {.data.event = E_STOP} ;
  uiss_stop();
  uiss_mqReceive(&msg);
  transit =&mySm[myState][msg.data.event];

  assert_int_equal(transit->action,A_STOP);
  assert_int_equal(transit->destinationState,S_DEATH);
}

/**
 * Suite de test pour le module.
 */
const struct CMUnitTest tests[] = 
{
	cmocka_unit_test(test_MachineEtatUiss_transition_1_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_1_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_2_sans_mq),
    //cmocka_unit_test(test_MachineEtatUiss_transition_2_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_3_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_3_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_4_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_4_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_5_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_5_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_6_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_6_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_7_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_7_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_8_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_8_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_9_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_9_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_10_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_10_avec_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_11_sans_mq),
    cmocka_unit_test(test_MachineEtatUiss_transition_11_avec_mq)
};



/**
 * Lancement de la suite de tests pour le module.
 */
// int main() 
// {
// 	return cmocka_run_group_tests_name("Test du module mod1", tests, set_up, tear_down);
// }

