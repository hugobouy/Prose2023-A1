/**
 * @file 	Employee.java
 * @brief	Classe d'un employé.
 * @author 	CASSAR Bastien
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

public class Employee
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief prénom.
     */
    private final String firstName;
    /**
     * @brief nom.
     */
    private final String name;
    /**
     * @brief identifiant.
     */
    private final int employeeID;
    /**
     * @brief rôle.
     */
    private final int role;
    /**
     * @brief heure de travail.
     */
    private final byte[][] workingHours;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur initialisant toutes les informations d'un employé.
     */
    public Employee(int employeeID, String name, String firstName, int role, byte[][] workingHours)
    {
        this.employeeID = employeeID;
        this.name = name;
        this.firstName = firstName;
        this.role = role;
        this.workingHours = workingHours;
    }
    /**
     * @brief obtient le nom d'un employé.
     * @return le nom.
     */
    public String getName()
    {
        return name;
    }
    /**
     * @brief obtient le prénom d'un employé.
     * @return le prénom.
     */
    public String getFirstName()
    {
        return firstName;
    }
    /**
     * @brief obtient le rôle d'un employé.
     * @return le rôle.
     */
    public int getRole()
    {
        return role;
    }
    /**
     * @brief obtient le rôle d'un employé.
     * @return le rôle.
     */
    public int getEmployeeID()
    {
        return employeeID;
    }
    /**
     * @brief Permet d'avoir la première valeur des heures de travail.
     * @return La premier valeur du tableau selon le jour de la semaine.
     */
    public byte getStart(int day)
    {
        return workingHours[day][0];
    }
    /**
     * @brief Permet d'avoir la deuxième valeur des heures de travail.
     * @param day [in] Détermine le jour de la semaine.
     * @return La destartuxième valeur du tableau selon le jour de la semaine.
     */
    public byte getEnd(int day)
    {
        return workingHours[day][1];
    }
}
