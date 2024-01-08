Code de production de la classe EmployeeManager


Ci dessous le main.c utilisé pour tester la classe lors de la production, utile notamment pour afficher la liste des employés (méthode afficherListeEmployés)

```
#include <stdlib.h>
#include <stdio.h>

#include "EmployeeManager.h"

void testAjoutEmploye();
void testAjout11Employes();
void testSuppressionEmploye();
void testGetEmployee();
void testSave();
void testLoad();

void afficherListeEmployes();

uint8_t myworkingHours[7][2] = {
    {25,8},
    {7,9},
    {4,1},
    {8,9},
    {12, 18},
    {0,7},
    {0,9}
};

int main()
{
    //testAjoutEmploye();
    testAjout11Employes();
    //testSuppressionEmploye();
    //testGetEmployee();
}

void afficherListeEmployes()
{
    Employee_t const* employeeList = getEmployeeList();
    if (employeeList != NULL) 
    {
        int i = 0;
        while(employeeList[i].employeeID != 0 && i != 10) 
        {
            Employee_t employee = employeeList[i];

            // Afficher les champs de l'employé
            printf("Employee #%d\n", i + 1);
            printf("Name: %s\n", employee.name);
            printf("First Name: %s\n", employee.firstName);
            printf("Role: %d\n", employee.role);
            printf("ID: %d\n", employee.employeeID);
            printf("Working Hours:\n");
            for (int j = 0; j < 7; j++) {
                printf("Day %d: Start = %d, End = %d\n", j + 1, employee.workingHours[j][0], employee.workingHours[j][1]);
            }
            printf("Picture: %s\n", employee.picture);
            i++;
        }
    } 
    else 
    {
        printf("Employee list is empty or unaccessible.\n");
    }
}

void testGetEmployee()
{
    addEmployee("a", "armand", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);
    addEmployee("b", "bernard", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo2.jpg", E_EVENING, myworkingHours);
    addEmployee("c", "claude", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo3.jpg", E_MORNING, myworkingHours);
    addEmployee("d", "didier", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo4.jpg", E_SECURITY, myworkingHours);
    addEmployee("e", "emile", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo5.jpg", E_SPECIAL, myworkingHours);
    addEmployee("f", "françois", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo6.jpg", E_DAY, myworkingHours);
    addEmployee("g", "guenael", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo7.jpg", E_DAY, myworkingHours);
    addEmployee("h", "hector", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo8.jpg", E_DAY, myworkingHours);
    addEmployee("i", "i", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo9.jpg", E_DAY, myworkingHours);
    addEmployee("j", "jerome", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo10.jpg", E_DAY, myworkingHours);

    deleteEmployee(1);
    deleteEmployee(2);
    deleteEmployee(3);

    addEmployee("a", "armand", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);
    addEmployee("b", "bernard", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo2.jpg", E_EVENING, myworkingHours);
    addEmployee("c", "claude", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo3.jpg", E_MORNING, myworkingHours);

    Employee_t employee = getEmployee(8);
    printf("ID : %d\n", employee.employeeID);
}

void testSuppressionEmploye()
{
    addEmployee("a", "armand", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);
    addEmployee("b", "bernard", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo2.jpg", E_EVENING, myworkingHours);
    addEmployee("c", "claude", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo3.jpg", E_MORNING, myworkingHours);
    addEmployee("d", "didier", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo4.jpg", E_SECURITY, myworkingHours);
    addEmployee("e", "emile", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo5.jpg", E_SPECIAL, myworkingHours);
    addEmployee("f", "françois", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo6.jpg", E_DAY, myworkingHours);
    addEmployee("g", "guenael", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo7.jpg", E_DAY, myworkingHours);
    addEmployee("h", "hector", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo8.jpg", E_DAY, myworkingHours);
    addEmployee("i", "i", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo9.jpg", E_DAY, myworkingHours);
    addEmployee("j", "jerome", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo10.jpg", E_DAY, myworkingHours);
    
    deleteEmployee(1);
    addEmployee("a", "armand", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);

    afficherListeEmployes();
}

void testAjoutEmploye()
{
    addEmployee("Chiron", "Paul", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);
    afficherListeEmployes();
}

void testAjout11Employes()
{
    addEmployee("a", "armand", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo.jpg", E_DAY, myworkingHours);
    addEmployee("b", "bernard", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo2.jpg", E_EVENING, myworkingHours);
    addEmployee("c", "claude", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo3.jpg", E_MORNING, myworkingHours);
    addEmployee("d", "didier", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo4.jpg", E_SECURITY, myworkingHours);
    addEmployee("e", "emile", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo5.jpg", E_SPECIAL, myworkingHours);
    addEmployee("f", "françois", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo6.jpg", E_DAY, myworkingHours);
    addEmployee("g", "guenael", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo7.jpg", E_DAY, myworkingHours);
    addEmployee("h", "hector", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo8.jpg", E_DAY, myworkingHours);
    addEmployee("i", "i", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo9.jpg", E_DAY, myworkingHours);
    addEmployee("j", "jerome", "/home/paul/Documents/S8_PROSE/explos_perso/production/photo10.jpg", E_DAY, myworkingHours);

    addEmployee("ne passe", "pas", "photo.jpg", E_DAY, myworkingHours);

    afficherListeEmployes();
}
```
