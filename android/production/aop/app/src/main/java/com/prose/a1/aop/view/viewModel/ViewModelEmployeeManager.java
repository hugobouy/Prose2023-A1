/**
 * @file 	ViewModelEmployeeManager.java
 * @brief	ViewModel de la gestion des employés
 * @author 	CASSAR Bastien & Mathis Moulin
 * @date 	08/06/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

package com.prose.a1.aop.view.viewModel;

import androidx.lifecycle.MutableLiveData;

import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;

public class ViewModelEmployeeManager extends androidx.lifecycle.ViewModel
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur modifier lors du changement de fragment
     */
    MutableLiveData<Integer> screenCalendar;
    /**
     * @brief valeur modifier lors du changement de fragment
     */
    MutableLiveData<Integer> screenEmployeeList;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur implémentant tous les mutableData possible lors de la connexion.
     */
    public ViewModelEmployeeManager()
    {
        this.screenCalendar = CacheEmployeeManager.INSTANCE.getScreenCalendar();
        this.screenEmployeeList = CacheEmployeeManager.INSTANCE.getScreenEmployeeList();
    }
    /**
     * @brief Retourne l'écran du calendrier
     * @return screenEmployeeList
     */
    public MutableLiveData<Integer> getScreenEmployeeList() {
        return screenEmployeeList;
    }
    /**
     * @brief Retourne l'écran du calendrier
     * @return screenCalendar
     */
    public MutableLiveData<Integer> getScreenCalendar() {
        return screenCalendar;
    }
}
