#include <pthread.h>
pthread_mutex_t employee_mutex;


#include "../../production/softSonnette/employeeManager.c"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "../cmocka/cmocka-x86_64/include/cmocka.h"

#define ONEDAY 1    //définit un jour fixe pour les tests, supposant que les heures de travail sont tous les jours les memes.

/**
 * @brief créé un mock de la fonction proxyConnectionManager_validatePass
 * 
 * @param passValidated 
 */
void __wrap_proxyGUI_setEmployeeList(uint8_t employeeID, char* name, char* surname, uint8_t role, Hour workingHours[7][2]);

void test_addEmployee(void **);

void test_addSecondEmployee(void **);

void test_deleteEmployee(void **);

void test_getEmployeeList(void **);

void test_getEmployee(void **);

void miseajourData();


void __wrap_proxyGUI_setEmployeeList(uint8_t employeeID, char* name, char* surname, uint8_t role, Hour workingHours[7][2])
{  
    function_called();
}

//************ données de test ******************//

    Hour workingHours[7][2];       
    Hour startHour; 
    Hour stopHour; 
    Picture testPicture; 

        //définition des variables de test pour l'employé Luc
    Hour workingHoursL[7][2];
    Hour startHourL; 
    Hour stopHourL;  
    Picture testPictureLuc; 

void miseajourData()
{

    //données pour l'employé Jean-Michel
    testPicture.filePath = "chemin/vers/picture";
    startHour.wh = 8;
    stopHour.wh  = 17;
    for(int i=0; i <=6; i++)
    {
        workingHours[i][0] = startHour;
        workingHours[i][1] = stopHour;

    }

    //données pour l'employé Luc
    testPictureLuc.filePath = "chemin/vers/pictureLuc";
    startHourL.wh = 7;
    stopHourL.wh  = 19;
    for(int i=0; i <=6; i++)
    {
        workingHoursL[i][0] = startHourL;
        workingHoursL[i][1] = stopHourL;

    }

    
}

void test_addEmployee(void **)
{
    miseajourData();

    //fonction à tester
    employeeManager_addEmployee("Michel", "Jean-Michel", testPicture, E_DAY, workingHours);
   

    for(int i=0; i <=MAX_EMPLOYEE; i++)
    {
        //on vérifie que l'employé à été installé dans la première case du tableau:
        if (i==0)
        {
            assert_string_equal(employeeList[i].name, "Michel");                                //nom correct
            assert_string_equal(employeeList[i].firstName, "Jean-Michel");                      //prenom correct
            assert_string_equal(employeeList[i].picture.filePath, "chemin/vers/picture");       //photo correcte
            assert_int_equal(employeeList[i].role, E_DAY);                                      //role correct
            assert_int_equal(employeeList[i].workingHours[ONEDAY][0].wh, startHour.wh);         //heure de début correcte
            assert_int_equal(employeeList[i].workingHours[ONEDAY][1].wh, stopHour.wh);          //heure de fin correcte
        }

        //on vérifie que l'employé n'a pas été placé dans une autre case:

        else
        {
            assert_string_not_equal(employeeList[i].name, "Michel");                                //nom pas dans une autre case
            assert_string_not_equal(employeeList[i].firstName, "Jean-Michel");                      //prenom pas dans une autre case
            assert_null(employeeList[i].picture.filePath);                                          //photo pas dans une autre case
            assert_int_not_equal(employeeList[i].role, E_DAY);                                      //role pas dans une autre case
            assert_int_not_equal(employeeList[i].workingHours[ONEDAY][0].wh, startHourL.wh);         //heure de début pas dans une autre case
            assert_int_not_equal(employeeList[i].workingHours[ONEDAY][1].wh, stopHourL.wh);          //heure de fin pas dans une autre case
        }
    }
}

void test_addSecondEmployee(void **)
{
    miseajourData();

    //fonction à tester
    
    employeeManager_addEmployee("Luc", "Skywalker", testPictureLuc, E_SPECIAL, workingHoursL);
    for(int i=0; i <=MAX_EMPLOYEE; i++)
    {
        if (i==0)           //Jean-Michel est bien sur la case zéro
        {
            assert_string_equal(employeeList[i].name, "Michel");                                //nom correct
            assert_string_equal(employeeList[i].firstName, "Jean-Michel");                      //prenom correct
            assert_string_equal(employeeList[i].picture.filePath, "chemin/vers/picture");       //photo correcte
            assert_int_equal(employeeList[i].role, E_DAY);                                      //role correct
            assert_int_equal(employeeList[i].workingHours[ONEDAY][0].wh, startHour.wh);         //heure de début correcte
            assert_int_equal(employeeList[i].workingHours[ONEDAY][1].wh, stopHour.wh);          //heure de fin correcte*/
        }
        if (i==1)           //Luc est bien sur la case un
        {
            assert_string_equal(employeeList[i].name, "Luc");                                    //nom correct
            assert_string_equal(employeeList[i].firstName, "Skywalker");                         //prenom correct
            assert_string_equal(employeeList[i].picture.filePath, "chemin/vers/pictureLuc");        //photo correcte
            assert_int_equal(employeeList[i].role, E_SPECIAL);                                   //role correct
            assert_int_equal(employeeList[i].workingHours[ONEDAY][0].wh, startHourL.wh);         //heure de début correcte
            assert_int_equal(employeeList[i].workingHours[ONEDAY][1].wh, stopHourL.wh);          //heure de fin correcte*/
        }
    }
}

void test_deleteEmployee(void **)
{
        miseajourData();


    EmployeeID id; id.ID =1;
    employeeManager_deleteEmployee(id); //enlève l'employé possédant l'ID 0

                    //Jean-Michel a bien été remercié définitivement.

    assert_string_not_equal(employeeList[0].name, "Michel");                                //nom a diaparu de sa case
    assert_string_not_equal(employeeList[0].firstName, "Jean-Michel");                      //prenom a diaparu de sa case
    assert_string_not_equal(employeeList[0].picture.filePath, testPicture.filePath);                                          //photo a diaparu de sa case
    assert_int_not_equal(employeeList[0].role, E_DAY);                                      //role a diaparu de sa case
     assert_int_not_equal(employeeList[0].workingHours[ONEDAY][0].wh, startHour.wh);         //heure de début a diaparu de sa case
    // assert_int_not_equal(employeeList[0].workingHours[ONEDAY][1].wh, stopHour.wh);          //heure de fin a diaparu de sa case
        
                    //Luc est bien passé en première case du tableau

    assert_string_equal(employeeList[0].name, "Luc");                                    //nom correct
    assert_string_equal(employeeList[0].firstName, "Skywalker");                         //prenom correct
    assert_string_equal(employeeList[0].picture.filePath, "chemin/vers/pictureLuc");     //photo correcte
    assert_int_equal(employeeList[0].role, E_SPECIAL);                                   //role correct
    assert_int_equal(employeeList[0].workingHours[ONEDAY][0].wh, startHourL.wh);         //heure de début correcte
    assert_int_equal(employeeList[0].workingHours[ONEDAY][1].wh, stopHourL.wh);          //heure de fin correcte
}


void test_getEmployeeList(void **)

{
    miseajourData();

    Employee_t *list = employeeManager_getEmployeeList();

    assert_string_equal(list[0].name, "Luc");                                    //nom correctement déplacé
    assert_string_equal(list[0].firstName, "Skywalker");                         //prenom correctement déplacé
    assert_string_equal(list[0].picture.filePath, "chemin/vers/pictureLuc");     //photo correctement déplacée
    assert_int_equal(list[0].role, E_SPECIAL);                                   //role correctement déplacé
    assert_int_equal(list[0].workingHours[ONEDAY][0].wh, startHourL.wh);         //heure de début correctement déplacée
    assert_int_equal(list[0].workingHours[ONEDAY][1].wh, stopHourL.wh);          //heure de fin correctement déplacée


}
void test_getEmployee(void **)  
{
    EmployeeID id;
    id.ID=1;


   Employee_t employeeRecu;
   employeeRecu = employeeManager_getEmployee(id);

    assert_string_equal(employeeRecu.name, "Luc");                                    //nom correctement déplacé
    assert_string_equal(employeeRecu.firstName, "Skywalker");                         //prenom correctement déplacé
    assert_string_equal(employeeRecu.picture.filePath, "chemin/vers/pictureLuc");     //photo correctement déplacée
    assert_int_equal(employeeRecu.role, E_SPECIAL);                                   //role correctement déplacé
    assert_int_equal(employeeRecu.workingHours[ONEDAY][0].wh, startHourL.wh);         //heure de début correctement déplacée
    assert_int_equal(employeeRecu.workingHours[ONEDAY][1].wh, stopHourL.wh);          //heure de fin correctement déplacée


}

void test_askEmployeeList(void **)
{
    //expect_function_call(__wrap_proxyGUI_setEmployeeList);
    employeeManager_askEmployeeList();
}

void test_load(void **)
{
    //Création du fichier d'employés pour tester la fonction
    FILE *file = fopen("employees.csv", "w");
    fprintf(file, "Kenobi,Obiwan,1,3,1,2,3,4,5,6,7,8\n");
    fclose(file);
    employeeManager_load();
    
    assert_string_equal(employeeList[0].name, "Kenobi");                    //prenom correct
    assert_string_equal(employeeList[0].firstName, "Obiwan");               //nom correct
    assert_int_equal(employeeList[0].role, 1);                              //role correct
    assert_int_equal(employeeList[0].employeeID.ID, 3);                     //id correcte
    assert_int_equal(employeeList[0].workingHours[ONEDAY][0].wh, 3);             //heure début correcte
    assert_int_equal(employeeList[0].workingHours[ONEDAY][1].wh, 4);             //heure fin correcte

    // Supprimer le fichier de test pour éviter des bugs
    remove("employees.csv");
}

void test_save(void **)
{
     // Remplir la liste d'employés avec des données de test
    Employee_t nouvelEmployee;

    nouvelEmployee.role = 1;
    nouvelEmployee.workingHours[ONEDAY][0].wh = 4;
    nouvelEmployee.workingHours[ONEDAY][1].wh = 12;
    employeeManager_addEmployee("Yoda", "Maitre", testPicture, nouvelEmployee.role, nouvelEmployee.workingHours);

    // Appeler la fonction à tester
    employeeManager_save();

    // Vérifier le contenu du fichier 
    employeeManager_load();
    
    assert_string_equal(employeeList[1].firstName, "Maitre");               //nom correct
    assert_string_equal(employeeList[1].name, "Yoda");                    //prenom correct
    assert_int_equal(employeeList[1].role, 1);                              //role correct
    assert_int_equal(employeeList[1].employeeID.ID, 2);                     //id correcte
    assert_int_equal(employeeList[1].workingHours[ONEDAY][0].wh, 4);             //heure début correcte
    assert_int_equal(employeeList[1].workingHours[ONEDAY][1].wh, 12);             //heure fin correcte
}

int main(void) {
    const struct CMUnitTest tests[] = {
        
        cmocka_unit_test(test_addEmployee),
        cmocka_unit_test(test_addSecondEmployee),
        cmocka_unit_test(test_deleteEmployee),
        cmocka_unit_test(test_getEmployeeList),
        cmocka_unit_test(test_getEmployee),             
        cmocka_unit_test(test_askEmployeeList),         
        cmocka_unit_test(test_load),                    
        cmocka_unit_test(test_save),                    	




    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
