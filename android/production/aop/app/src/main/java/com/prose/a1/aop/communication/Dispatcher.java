/**
 * @file 	Dispatcher.java
 * @brief	Classe permettant de rediriger les informations selon la commande
 * @author 	CASSAR Bastien
 * @date 	08/05/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */
package com.prose.a1.aop.communication;

import android.util.Log;

import com.prose.a1.aop.model.Door;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;

public class Dispatcher
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief Index cmdID
     */
    private final static byte CMD_ID = 0;
    /**
     * @brief valeur premier argument en colonne
     */
    private final static byte COLONNE_FIRST_ARG = 3;
    /**
     * @brief valeur premier argument en ligne
     */
    private final static byte LIGNE_FIRST_ARG = 0;
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief Cas d'une erreur de connexion.
     */
    private final static byte ERROR_CONNECTION = 0;
    /**
     * @brief Réponse à l'envoi d'un mot de passe.
     */
    private final static byte VALIDATE_PASS = 3;
    /**
     * @brief Réponse à la demande d'accès à la liste des employés.
     */
    private final static byte SET_EMPLOYEE_LIST = 11;
    /**
     * @brief Réponse à la demande de l'état de la Porte.
     */
    private final static byte UPDATE_DOOR_STATE = 7;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Dirige le protocole selon l'identifiant de la commande
     * @param messageDecode permet d'avoir toutes les informations de la trame reçu
     */
    public void dispatch(byte[][] messageDecode)
    {
        switch (messageDecode[CMD_ID][CMD_ID])
        {
            case ERROR_CONNECTION:
                ConnectionManager.INSTANCE.checkConnection();
                break;
            case SET_EMPLOYEE_LIST:
                if (convertBinaireToString(messageDecode[COLONNE_FIRST_ARG][LIGNE_FIRST_ARG]).equals("0"))
                {
                    Log.d(TAG, "Ajout d'un employé fin de data");
                    if(Communication.INSTANCE.askListEmployee) CacheEmployeeManager.INSTANCE.askCalendar();
                    else CacheEmployeeManager.INSTANCE.askEmployeeList();
                }
                else
                {
                    Log.d(TAG, "Ajout d'un employé valeur : " + convertBinaireToString(messageDecode[3][0]));
                    CacheEmployeeManager.INSTANCE.askEmployeeList(messageDecode);
                }
                break;
            case VALIDATE_PASS:
                Log.d(TAG, "Valid pass : " + messageDecode[COLONNE_FIRST_ARG][LIGNE_FIRST_ARG]);
                ConnectionManager.INSTANCE.validatePass(messageDecode[COLONNE_FIRST_ARG][LIGNE_FIRST_ARG]);
                break;
            case UPDATE_DOOR_STATE:
                Log.e(TAG, "dispacther UPDATE_DOOR_STATE : " + messageDecode[3][0]);
                Door.INSTANCE.setDoorState(messageDecode[COLONNE_FIRST_ARG][LIGNE_FIRST_ARG]);
                break;
            default:
                Log.d(TAG, " non traité pour l'instant cmdId n° : " +  messageDecode[COLONNE_FIRST_ARG][LIGNE_FIRST_ARG]);
                break;
        }
    }
    /**
     * @brief converti le binaire en String
     * @param messageEncode [in] binaire à convertir
     */
    private String convertBinaireToString(byte messageEncode)
    {
        return String.format("%c", messageEncode);
    }
}
