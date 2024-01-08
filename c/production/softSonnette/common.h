/**
 * @file 	common.h
 * @brief 	FIchier commun partagé à tous les
 * @author 	CHIRON Paul
 * @date 	11/05/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

#ifndef COMMON_H
#define COMMON_H

#define DEBUG 0

#define SERVER_PORT 1253
#define MAX_PENDING_CONNECTIONS 10
#define MAX_TCP_LENGTH 1500
#define SP_LENGTH 2

#define LEN_MAX_NAME 12
#define LEN_MAX_FIRSTNAME 12

/*
 * INCLUDES
 */

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>


extern pthread_barrier_t start_barrier;
extern pthread_barrier_t stop_barrier;
extern pthread_barrier_t quit_barrier;
extern pthread_mutex_t employee_mutex;

typedef enum 
{
    MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY,SUNDAY
}Day;


typedef struct 
{
    char * filePath;
}Picture;


 typedef struct 
 {
    
 uint8_t ID;

 }EmployeeID;


typedef struct 
{
    uint16_t year;
    uint8_t month,day,hour,minute,second;
    Day dayOfWeek;

}Time;


typedef struct 
{
    uint8_t wh;
    uint8_t hour, minute, second;

}Hour;

/**
 * @brief Enumération correspondant aux différents rôles pris par les employés
 */
typedef enum 
{
    E_MORNING = 0,
    E_DAY,
    E_EVENING,
    E_SECURITY,
    E_SPECIAL
}Role_e;

/**
 * @brief Structure de données correspondant aux informations liées à un employé
 */
typedef struct {
    char name[LEN_MAX_NAME + 1];
    char firstName[LEN_MAX_FIRSTNAME + 1];
    Role_e role;
    EmployeeID employeeID;
    Hour workingHours[7][2];
    Picture picture;
}Employee_t;

/**
 * @brief Enumération des identifiants de commande entre AOP et softSonnette
 * 
 */
typedef enum {
    AOP_CMD_ERR = 0,
    AOP_CMD_SUBSCRIBE_TO_VIDEO_STREAM,
    AOP_CMD_ASK_CHECK_PASS,
    AOP_CMD_VALIDATE_PASS,
    AOP_CMD_SET_TIME,
    AOP_CMD_ASK_OPEN_DOOR,
    AOP_CMD_ASK_DOOR_STATE,
    AOP_CMD_UPDATE_DOOR_STATE,
    AOP_CMD_ADD_EMPLOYEE,
    AOP_CMD_DELETE_EMPLOYEE,
    AOP_CMD_ASK_EMPLOYEE_LIST,
    AOP_CMD_SET_EMPLOYEE_LIST,
    AOP_NB_CMD
} Aop_SsCommand;

/**
 * @brief Enumération des identifiants de commande entre softPorte et softSonnette
 * 
 */
typedef enum {
    SP_CMD_ERR = 0,
    SP_CMD_ASK_OPEN_DOOR,
    SP_CMD_UPDATE_DOOR_STATE_TRUE,
    SP_NONE,
    SP_CMD_ASK_DOOR_STATE,
    SP_CMD_SIGNAL_FACE_NOT_ALLOWED,
    SP_CMD_LAUNCH_SP,
    SP_CMD_QUIT_SP,
    SP_CMD_ASK_START_COM,
    SP_CMD_ACK_START_COM,
    SP_CMD_UPDATE_DOOR_STATE_FALSE,
    SP_NB_CMD
} Sp_SsCommand;

/**
 * @brief Enumération des targets pour la com entre softPorte et softSonnette
 * 
 */
typedef enum {
    SOFTS_ID = 0,
    AOP_ID,
    SOFTP_ID
} Sp_Target;

/**
 * @brief Type du password qui est un tableau de char.
 * 
 */
typedef char Password[];

#if DEBUG
#define TRACE(fmt, ...) do { \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    fflush (stderr); \
    } while(0);
#define ASSERT_PRINTERROR(assertion) do { \
    if (!(assertion))  \
      {perror("Erreur"); \
        assert(assertion);} \
  } while(0);
#define STOP_ON_ERROR(error_condition) do { \
	if(error_condition) { \
		fprintf (stderr, "*** Error (%s) at \
		%s:%d\nExiting\n", \
		#error_condition, __FILE__, __LINE__); \
		perror (""); \
		exit (EXIT_FAILURE); }                     \
    } while (0);

#else
#define TRACE(fmt, ...)
#define ASSERT_PRINTERROR(assertion)
#define TRACE_PUML(fmt, ...)
#define TRACE_PUML_START
#define TRACE_PUML_END

#define STOP_ON_ERROR(error_condition)
#endif

#endif //COMMON_H