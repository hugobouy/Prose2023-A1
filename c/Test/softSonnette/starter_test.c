#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <pthread.h>
#include "cmocka.h"

//#include "../../production/softSonnette/starter.c"

pthread_barrier_t start_barrier; //barrière de démarrage de l'application. Synchronise les appels aux starts des objets
pthread_barrier_t stop_barrier; //barrière d'arrêt de l'application. Synchronise les appels aux stop des objets
pthread_barrier_t quit_barrier;
pthread_mutex_t employee_mutex;