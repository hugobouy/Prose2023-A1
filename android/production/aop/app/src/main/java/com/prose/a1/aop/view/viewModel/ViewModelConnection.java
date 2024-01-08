/**
 * @file 	ViewModelConnection.java
 * @brief	ViewModel de la connexion
 * @author 	CASSAR Bastien
 * @date 	15/05/2023
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

import com.prose.a1.aop.model.connectionManager.ConnectionManager;

public class ViewModelConnection extends androidx.lifecycle.ViewModel
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur de la validation du mot de passe
     */
    MutableLiveData<Integer> validPass;
    /**
     * @brief valeur modifier en cas d'erreur de connexion
     */
    MutableLiveData<Integer> errorConnection;
    /**
     * @brief valeur modifier en cas de format d'adresse IP incorrect
     */
    MutableLiveData<Integer> wrongFormatIp;
    /**
     * @brief valeur modifier en cas d'attente lors de la connexion
     */
    MutableLiveData<Integer> waitConnect;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur implémentant tous les mutableData possible lors de la connexion.
     */
    public ViewModelConnection()
    {
        this.validPass = ConnectionManager.INSTANCE.getValidPass();
        this.errorConnection = ConnectionManager.INSTANCE.getErrorConnection();
        this.wrongFormatIp = ConnectionManager.INSTANCE.getWrongFormatIp();
        this.waitConnect = ConnectionManager.INSTANCE.getWaitConnect();
    }
    /**
     * @brief Retourne la valeur de validation du mot de passe
     * @return validPass
     */
    public MutableLiveData<Integer> getValidPass()
    {
        return validPass;
    }
    /**
     * @brief Retourne l'état de la connexion
     * @return errorConnnection
     */
    public MutableLiveData<Integer> getErrorConnection() {
        return errorConnection;
    }
    /**
     * @brief Retourne l'état du format de l'adresse IP
     * @return wrongFormatIp
     */
    public MutableLiveData<Integer> getWrongFormatIp() {
        return wrongFormatIp;
    }
}
