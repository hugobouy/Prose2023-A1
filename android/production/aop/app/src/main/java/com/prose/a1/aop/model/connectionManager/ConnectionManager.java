/**
 * @file 	ConnectionManager.java
 * @brief	Classe permettant de gérer les élements de connexion.
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
package com.prose.a1.aop.model.connectionManager;

import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import com.prose.a1.aop.communication.Communication;
import java.util.Calendar;

public class ConnectionManager
{

    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur activation des liveData
     */
    private static final int ACTIVATE = 1;
    /**
     * @brief valeur de désactivation des liveData
     */
    private static final int DEACTIVATION = 0;
    /**
     * @brief tag pour les debbugs
     */
    private static final String TAG = "TAG";
    /**
     * @brief compteur permettant de savoir dans quel fragment on se situe en cas d'erreur de connexion.
     */
    public int cnt = 1;
    /**
     * @brief variable permettant de savoir le nombre de fois que l'utilisateur a essayé de se connecter sans succès.
     */
    private int nbClick = 0;
    /**
     * @brief LiveData pour l'échec de la connexion au serveur.
     */
    private final MutableLiveData<Integer> errorConnection = new MutableLiveData<>();
    /**
     * @brief LiveData pour une adresse Ip incorrect.
     */
    private final MutableLiveData<Integer> wrongFormatIp = new MutableLiveData<>();
    /**
     * @brief LiveData pour afficher l'attente de connexion.
     */
    private final MutableLiveData<Integer> waitConnect = new MutableLiveData<>();
    /**
     * @brief LiveData pour la validation du mot de passe.
     */
    private final MutableLiveData<Integer> validPass = new MutableLiveData<>();
    /**
     * @brief Singleton de la classe.
     */
    public static ConnectionManager INSTANCE = new ConnectionManager();

    /*
     * FUNCTIONS
     */
    /**
     * @brief Met à jour l'échec de la connexion.
     * @return si il y a un échec lors de la connexion.
     */
    public MutableLiveData<Integer> getErrorConnection() {
        return errorConnection;
    }
    /**
     * @brief Dit si le format de l'adresse est incorrect.
     * @return si il y a une adresse Ip incorrect.
     */
    public MutableLiveData<Integer> getWrongFormatIp() {
        return wrongFormatIp;
    }
    /**
     * @brief Lance la popUp d'attente de connexion.
     * @return la valeur d'attente.
     */
    public MutableLiveData<Integer> getWaitConnect() {
        Log.i(TAG, "getWaitingConnect");
        return waitConnect;
    }
    /**
     * @brief Met à jour la validation du mot de passe.
     * @return si le mot de passe est correct ou non.
     */
    public MutableLiveData<Integer> getValidPass() {
        return validPass;
    }
    /**
     * @brief Vérifie si l'adresse Ip correspond au ban format.
     * @param myIp [in] adresse entrée par l'utilisateur.
     * @return true si l'adresse est au format X.X.X.X
     */
    private boolean checkIp(String myIp)
    {
        String ipPattern = "^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                + "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                + "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                + "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";
        return myIp.matches(ipPattern);
    }
    /**
     * @brief Prépare et envoie un message au serveur.
     * @param myPass [in] mot de passe entré par l'utilisateur.
     * @param myIp [in] valeur de l'adresse ip entrée par l'utilisateur.
     */
    public void askConnection(String myIp, String myPass)
    {
        if (checkIp(myIp))
        {
            nbClick+=1;
            Log.d(TAG, "askConnection");
            getWaitConnect().postValue(1);
            if (nbClick==1)  Communication.INSTANCE.beginConnection(myIp, myPass);
            else Communication.INSTANCE.askCheckPass(myPass);
        }
        else wrongFormatIp();
    }
    /**
     * @brief Retourne si le mot de passe est correcte
     * @param valid [in] valeur envoyée par le serveur afin de savoir si le mot de passe est correct
     */
    public void validatePass(byte valid)
    {
        if (convertBinaireToString(valid).equals("1")) getValidPass().postValue(ACTIVATE);
        else
        {
            Communication.INSTANCE.endConnection();
            getValidPass().postValue(DEACTIVATION);
        }
    }
    /**
     * @brief initialise les valeurs du viewModelConnection.
     */
    public void initPass()
    {
        getValidPass().postValue(3);
        getWaitConnect().postValue(DEACTIVATION);
    }
    /**
     * @brief Récupère l'heure sur l'AOP
     */
    public void getLocalTime()
    {
        // Obtenir le temps actuel
        Calendar calendar = Calendar.getInstance(); // Obtenir une instance de Calendar avec la date et l'heure actuelles
        byte[] dateArray = new byte[7]; // Créer un tableau de byte de 7 éléments

        int year = calendar.get(Calendar.YEAR);
        dateArray[0] = (byte) (year >>> 8); // Octet le plus significatif de l'année
        dateArray[1] = (byte) year; // Récupérer l'année
        dateArray[2] = (byte) (calendar.get(Calendar.MONTH) + 1); // Récupérer le mois (attention : les mois dans Calendar commencent à 0)
        dateArray[3] = (byte) calendar.get(Calendar.DAY_OF_MONTH); // Récupérer le jour
        dateArray[4] = (byte) calendar.get(Calendar.HOUR_OF_DAY); // Récupérer l'heure
        dateArray[5] = (byte) calendar.get(Calendar.MINUTE); // Récupérer les minutes
        dateArray[6] = (byte) calendar.get(Calendar.SECOND); // Récupérer les secondes
        // Afficher le temps actuel
        Communication.INSTANCE.setCurrentTime(dateArray);
    }
    /**
     * @brief initialise le viewModel afin de gérer l'échec de la connexion.
     */
    public void initErrorConnection()
    {
        this.errorConnection.postValue(DEACTIVATION);
    }
    /**
     * @brief Averti d'une erreur lors de la connexion.
     */
    public void checkConnection()
    {
        nbClick = DEACTIVATION;
        Log.e(TAG, "valeur du compteur dans checkConnection dans ConnectionManager : " + getErrorConnection().getValue());
        getErrorConnection().postValue(cnt);
    }
    /**
     * @brief Averti d'une erreur de format de l'adresse IP.
     */
    private void wrongFormatIp()
    {
        getWrongFormatIp().postValue(ACTIVATE);
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
