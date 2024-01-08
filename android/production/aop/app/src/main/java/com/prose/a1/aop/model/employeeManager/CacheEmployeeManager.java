/**
 * @file 	CacheEmployeeManager.java
 * @brief	Classe permettant de stocker les informations des employés.
 * @author 	CASSAR Bastien & Mathis Moulin
 * @date 	03/06/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */
package com.prose.a1.aop.model.employeeManager;


import androidx.lifecycle.MutableLiveData;

import java.util.ArrayList;
import java.util.List;

public class CacheEmployeeManager
{
    /*
     * LOCAL VARIABLES
     */
    public boolean callback = false;
    /**
     * @brief valeur activation des liveData
     */
    private static final int ACTIVATE = 1;
    /**
     * @brief valeur de désactivation des liveData
     */
    private static final int DEACTIVATION = 0;
    /**
     * @brief valeur du rôle
     */
    private static final int ROLE = 6;
    /**
     * @brief valeur du prénom
     */
    private static final int FIRSTNAME = 5;
    /**
     * @brief valeur du nom
     */
    private static final int NAME = 4;
    /**
     * @brief valeur de l'id
     */
    private static final int ID = 3;
    /**
     * @brief index de la valeur max
     */
    private static final int MAX = 1;
    /**
     * @brief index de la valeur min
     */
    private static final int MIN = 0;
    /**
     * @brief nombre de jours en une semaine.
     */
    private static final int NB_DAYS = 7;
    /**
     * @brief LiveData pour l'affiche du calendrier.
     */
    private final MutableLiveData<Integer> screenCalendar = new MutableLiveData<>();
    /**
     * @brief LiveData pour l'affichage de la liste des employés.
     */
    private final MutableLiveData<Integer> screenEmployeeList = new MutableLiveData<>();
    /**
     * @brief singleton.
     */
    public static CacheEmployeeManager INSTANCE = new CacheEmployeeManager();
    /**
     * @brief Liste des employés.
     */
    private final List<Employee> employes;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Affiche l'écran de la liste des employés.
     * @return screenEmployeeList.
     */
    public MutableLiveData<Integer> getScreenEmployeeList()
    {
        return screenEmployeeList;
    }
    /**
     * @brief Affiche l'écran calendrier.
     * @return screenCalendar.
     */
    public MutableLiveData<Integer> getScreenCalendar()
    {
        return screenCalendar;
    }
    /**
     * @brief Constructeur.
     */
    public CacheEmployeeManager()
    {
        employes = new ArrayList<>();
    }
    /**
     * @brief Permet d'obtenir tous les noms des employés.
     * @return Une liste de noms.
     */
    public List<String> getNameListEmployee()
    {
        List<String> employeeNames = new ArrayList<>();
        for (Employee employee : employes)
        {
            String name = employee.getName();
            employeeNames.add(name);
        }
        return employeeNames;
    }
    /**
     * @brief Permet d'obtenir tous les prénoms des employés.
     * @return Une liste de noms.
     */
    public List<String> getFirstNameListEmployee()
    {
        List<String> employeeFirstNames = new ArrayList<>();
        for (Employee employee : employes)
        {
            String firstName = employee.getFirstName();
            employeeFirstNames.add(firstName);
        }
        return employeeFirstNames;
    }
    /**
     * @brief Permet d'obtenir tous les prénoms des employés.
     * @return Une liste de noms.
     */
    public List<Integer> getRoleListEmployee()
    {
        List<Integer> employeeRole = new ArrayList<>();
        for (Employee employee : employes)
        {
            int role = employee.getRole();
            employeeRole.add(role);
        }
        return employeeRole;
    }
    /**
     * @brief Permet d'obtenir tous les ids des employés.
     * @return Une liste de noms.
     */
    public List<Integer> getIdEmployee()
    {
        List<Integer> employeeIds = new ArrayList<>();
        for (Employee employee : employes)
        {
            int id = employee.getEmployeeID();
            employeeIds.add(id);
        }
        return employeeIds;
    }
    /**
     * @brief Ajout d'un nouvel employé.
     * @param employeeID [in] identifiant d'un employé.
     * @param firstName [in] prénom de l'employé.
     * @param name [in] nom de famille de l'employé.
     * @param role [in] rôle de l'employé.
     * @param workingHours [in] heure de travail de l'employé.
     */
    public void setDataEmployee(int employeeID, String name, String firstName, int role, byte[][] workingHours)
    {
        employes.add(new Employee(employeeID, name, firstName,role, workingHours));
    }
    /**
     * @brief Ajoute un employé dans la liste et met en ordre le tableau des workingHours.
     * @param mes Données envoyées par le serveur.
     */
    public void askEmployeeList(byte[][] mes)
    {
        int k = 0;
        byte[][] workTime = new byte[NB_DAYS][2];
        for (int i = 0; i<NB_DAYS*2; i+=2)
        {
            k++;
            for (int j=0; j<1; j++)
            {
                workTime[k-1][j] = mes[NB_DAYS][i];
                workTime[k-1][j+1] = mes[NB_DAYS][1+i];
            }
        }
        int employeeID = mes[ID][0] - '0';
        int role = mes[ROLE][0] - '0';
        setDataEmployee(employeeID,convertBinaireToString(mes[NAME]),convertBinaireToString(mes[FIRSTNAME]), role, workTime);
    }
    /**
     * @brief converti le binaire en String
     * @param messageEncode [in] binaire à convertir
     */
    private String convertBinaireToString(byte[] messageEncode)
    {
        StringBuilder sb = new StringBuilder();
        for (byte b : messageEncode) {
            sb.append(String.format("%c", b));
        }
        return sb.toString();
    }
    /**
     * @brief Permet de mettre les heures dans l'ordre croissant afin de construire le calendrier comme il le faut.
     * @param name [in] nom de l'employé a affiché.
     * @return un tableau en deux dimension pour avoir les heures de début et les heures de fin de chaque jours d'un employé.
     */
    public int[][] askCalendarEmployee(String name)
    {
        int[][] index = new int[NB_DAYS][2];
        for (Employee employee : employes)
        {
            if (name.equals(employee.getName()))
            {
                for (int j = 0; j<NB_DAYS; j++)
                {
                    if (employee.getStart(j) > (employee.getEnd(j)))
                    {
                        index[j][MAX] = employee.getStart(j);
                        index[j][MIN] = employee.getEnd(j);
                    }
                    else if(employee.getStart(j) < (employee.getEnd(j)))
                    {
                        index[j][MIN] = employee.getStart(j);
                        index[j][MAX] = employee.getEnd(j);
                    }
                    else
                    {
                        index[j][MIN] = index[j][MAX] = 0;
                    }
                }
            }
        }
        return index;
    }
    /**
     * @brief supprime les données enregistrés dans l'application
     */
    public void deleteCache()
    {
        employes.clear();
    }
    /**
     * @brief demande l'affichage de l'écran calendrier
     */
    public void askCalendar()
    {
        getScreenCalendar().postValue(ACTIVATE);
    }
    /**
     * @brief demande l'affichage de l'écran liste employé
     */
    public void askEmployeeList()
    {
        getScreenEmployeeList().postValue(ACTIVATE);
    }
    /**
     * @brief réinitialise la liveDate à 0.
     */
    public void initAskEmployee()
    {
        this.screenEmployeeList.postValue(DEACTIVATION);
        this.screenCalendar.postValue(DEACTIVATION);
    }
}
