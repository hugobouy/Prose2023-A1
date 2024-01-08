/**
 * @file 	employeeManager.c
 * @brief 	Programme permettant la gestion des employés 
 * @author 	Paul CHIRON
 * @date 	09/05/2023
 *
 * \b License
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

/*
 * INCLUDES
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>
#include <unistd.h>
#include <jpeglib.h>
#include "employeeManager.h"
#include "proxyGUI.h"
//#include "common.h"

/*
 * LOCAL FUNCTIONS
 */

/*
 * LOCAL VARIABLES
 */

static Employee_t employeeList[MAX_EMPLOYEE];
static uint8_t IDcounter;

 /*******************************************************************************************/

void employeeManager_addEmployee(char name[], char firstName[], Picture picture, Role_e role, Hour workingHours[7][2])
{
    pthread_mutex_lock(&employee_mutex);

    int index = 0;
    while (index < MAX_EMPLOYEE && employeeList[index].employeeID.ID != 0) //Récupère le nombre d'employés dans la liste
    {
        index++;
    }

    if (index < MAX_EMPLOYEE) //Remplissage de la liste avec une structure Employee_t
    {
        Employee_t* employee = &employeeList[index];
        employee->employeeID.ID = ++IDcounter;
        strcpy(employee->name, name);
        strcpy(employee->firstName, firstName);
        employee->role = role;
        employee->picture.filePath = malloc((strlen(picture.filePath) + 1) * sizeof(char));
        strcpy(employee->picture.filePath, picture.filePath);

        memcpy(employee->workingHours, workingHours, 7 * 2 * sizeof(Hour));

        TRACE("Employee added\n");
    } else {
        fprintf(stderr, "Error addEmployee: employeeList is full\n");
    }

    pthread_mutex_unlock(&employee_mutex);
}


void employeeManager_deleteEmployee(EmployeeID employeeID)
{
    pthread_mutex_lock(&employee_mutex);

    if(employeeID.ID > 0 && employeeID.ID <= IDcounter) //Validité de l'index
    {
        bool found = false;
        for(int i = 0; i < employeeID.ID; i++) //Parcours de la liste
        {
            if(employeeList[i].employeeID.ID == employeeID.ID)
            {
                TRACE("path : %s\n", employeeList[i].picture.filePath);
                if(remove(employeeList[i].picture.filePath) != 0)
                {
                    perror("Remove image error\n");
                }
                found = true;
                for (int j = i + 1; j < MAX_EMPLOYEE; j++) //Décalage des éléments de la liste après l'élément supprimé
                {
                    employeeList[j - 1] = employeeList[j];
                    if (employeeList[j].employeeID.ID == 0) break; 
                }
                employeeList[MAX_EMPLOYEE - 1].employeeID.ID = 0; //Empêche potentiel doublon du dernier élément si la liste était pleine aavnt la suppression
                TRACE("Employee with ID %d removed\n", employeeID.ID);
            }
        }
        if(!found)
        {
            TRACE("Employee with ID %d not found\n", employeeID.ID);
        }
    }
    else
    {
        fprintf(stderr, "Error deleteEmployee : incorrect index\n");
    }

    pthread_mutex_unlock(&employee_mutex);
}

Employee_t* employeeManager_getEmployeeList(void)
{
    return employeeList;
}

Employee_t employeeManager_getEmployee(EmployeeID employeeID)
{
    pthread_mutex_lock(&employee_mutex);

    Employee_t employee;
    if(employeeID.ID > 0 && employeeID.ID <= IDcounter) //Validité de l'index
    {
        bool found = false;
        for(int i = 0; i < employeeID.ID; i++) //Parcours de la liste
        {
            if(employeeList[i].employeeID.ID == employeeID.ID)
            {
                employee = employeeList[i];
                found = true;
            }
        }
        if(!found)
        {
            TRACE("Employee with ID %d not found\n", employeeID.ID);
            employee.employeeID.ID = 0;
        }
    }
    else
    {
        fprintf(stderr, "Error getEmployee : incorrect index\n");
        employee.employeeID.ID = 0;
    }
    TRACE("employee returned\n");

    pthread_mutex_unlock(&employee_mutex);

    return employee;
}

void employeeManager_askEmployeeList()
{

    for(int i = 0; i < MAX_EMPLOYEE; i++)
    {
        if(employeeList[i].employeeID.ID != 0) //pour chaque employé valable de la liste, on envoie à AOP une trame
        {
            Employee_t employee = employeeManager_getEmployee(employeeList[i].employeeID);
            usleep(10000);
            proxyGUI_setEmployeeList(employee.employeeID.ID, employee.name, employee.firstName, employee.role, employee.workingHours);
        }
    }
    //signale à AOP la fin de la liste
    usleep(10000);
    proxyGUI_setEmployeeList(0, NULL, NULL, 0, 0);

}

void employeeManager_storeImage(char* imageBytes, char* filePath)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filePath, "wb")) == NULL) {
        fprintf(stderr, "Error opening output jpeg file %s\n", filePath);
        return;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = PHOTO_WIDTH;
    cinfo.image_height = PHOTO_HEIGHT;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = PHOTO_WIDTH * 3;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)&imageBytes[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}

void employeeManager_load()
{
    pthread_mutex_lock(&employee_mutex);

    FILE *file = fopen("employees.csv", "r"); //Ouverture du fichier en mode lecture
    if (file == NULL) {
        fprintf(stderr, "Error load : cannot opening file for reading\n");
        return;
    }

    Employee_t employee;
    int index = 0;
    int maxId = 1;
    while (fscanf(file, "%12[^,],%12[^,],%u,%hhu", employee.name, employee.firstName, &employee.role, &employee.employeeID.ID) == 4) { //Valeur de retour de fscanf = éléments correctement lus
        for (int j = 0; j < 7; j++) {
            fscanf(file, ",%hhu,%hhu", &employee.workingHours[j][0].wh, &employee.workingHours[j][1].wh);
        }

        char picture[100]; //Manipulation du champ picture, pointeur sur char

        fscanf(file, ",%[^\n]\n", picture);

        employee.picture.filePath = (char*)malloc((strlen(picture) + 1) * sizeof(char));

        if(employee.employeeID.ID > maxId)
        {
            maxId = employee.employeeID.ID;
        }
        strcpy(employee.picture.filePath, picture);

        employeeList[index] = employee; //Remplissage de la liste
        index++;
        if (index == MAX_EMPLOYEE) {
            break;
        }
        TRACE("LOADED\n");
    }
    IDcounter = maxId;

    fclose(file);
    TRACE("Employee list loaded\n");

    pthread_mutex_unlock(&employee_mutex);
}

void employeeManager_save()
{
    pthread_mutex_lock(&employee_mutex);

    FILE *file = fopen("employees.csv", "w"); //Ouverture du fichier en mode écriture
    if (file == NULL) {
        fprintf(stderr, "Error save : cannot opening file for writing\n");
        return;
    }

    for (int i = 0; i < MAX_EMPLOYEE; i++) { //Parcours la liste 
        if (employeeList[i].employeeID.ID != 0) {
            fprintf(file, "%s,%s,%d,%d", employeeList[i].name, employeeList[i].firstName, employeeList[i].role, employeeList[i].employeeID.ID);

            for (int j = 0; j < 7; j++) {
                fprintf(file, ",%d,%d", employeeList[i].workingHours[j][0].wh, employeeList[i].workingHours[j][1].wh);
            }

            fprintf(file, ",%s\n", employeeList[i].picture.filePath);
            free(employeeList[i].picture.filePath);
        }
    }

    fclose(file);
    TRACE("Employee list saved\n");

    pthread_mutex_unlock(&employee_mutex);
}
