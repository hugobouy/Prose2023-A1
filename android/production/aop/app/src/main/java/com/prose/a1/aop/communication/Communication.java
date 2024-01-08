/**
 * @file 	Communication.java
 * @brief	Classe permettant de faire la communication
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
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Base64;

public class Communication implements PostmanSoftSonnette.ConnectionObserver
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief valeur du mot de passe entré par l'utilisateur.
     */
    private String pass;
    /**
     * @brief valeur de timeout du ping
     */
    private static final int TIMEOUT = 2000;
    /**
     * @brief Permet d'obtenir la taille de la trame
     */
    private static final byte TRAME_SIZE = 2;
    /**
     * @brief Variable stockant la valeur de l'adresse du PORT.
     */
    private static final int SERVEUR_PORT = 1253;
    /**
     * @brief Crée une instance de la classe connection.
     */
    private final PostmanSoftSonnette postmanSoftSonnette;
    /**
     * @brief Crée une instance de la classe trame.
     */
    private final Protocol protocol;
    /**
     * @brief Crée une instance de la classe dispatcher.
     */
    private final Dispatcher dispatcher;
    /**
     * @brief Permet de lancer ou arrêter le while dans le run
     */
    private boolean isConnected = false;
    /**
     * @brief Variable permettant de connaître l'écran correspondant à la demande de l'utilisateur.
     */
    public boolean askListEmployee = false;
    /**
     * @brief Singleton de la classe.
     */
    public static Communication INSTANCE = new Communication();

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    private Communication()
    {
        this.postmanSoftSonnette = new PostmanSoftSonnette();
        this.protocol = new Protocol();
        this.dispatcher = new Dispatcher();
        this.postmanSoftSonnette.addObserver(this);
    }
    /**
     * @brief Lance la connexion.
     */
    public void beginConnection(String myIp, String myPass)
    {
        this.pass = myPass;
        this.postmanSoftSonnette.connectToServer(myIp, SERVEUR_PORT, TIMEOUT);
    }
    /**
     * @brief Lance la lecture et le décode du message receptionné.
     */
    public void run()
    {
        //Lance le thread faisant la lecture des messages venant du serveur.
        Thread thread = new Thread(() ->
        {
            while (isConnected)
            {
                try
                {
                    ByteBuffer buffer;
                    buffer = ByteBuffer.wrap(postmanSoftSonnette.readMsg(TRAME_SIZE));
                    short size = buffer.getShort();
                    byte[] msg = postmanSoftSonnette.readMsg((short) (size-2));
                    byte[] sizeArray = short2ByteArray(size);
                    byte[][] decode = protocol.decodeMessage(sizeArray, msg);
                    dispatcher.dispatch(decode);
                }
                catch (Exception e)
                {
                    Log.i(TAG, "Echec lors de la lecture du message (" + e.getMessage() + ")");
                }
            }
        });
        thread.start();
    }
    /**
     * @brief Converti un short en un tableau de byte
     * @param shrt [in] valeur a convertir
     */
    private byte[] short2ByteArray(short shrt)
    {
        return new byte[]{(byte)(shrt>>>8),(byte)shrt};
    }
    /**
     * @brief envoie le mot de passe entré par l'utilisateur
     * @param myPass mot de passe entré
     */
    public void askCheckPass(String myPass)
    {
        //Converti le message reçu.
        byte[] val = String.format("%s", myPass).getBytes();
        //Crée la trame pour le message puis l'envoie.
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_CONNECTION, this.protocol.NB_ARG_CONNECTION, val));
    }
    /**
     * @brief Termine la communication
     */
    public void endConnection()
    {
        isConnected = false;
        this.postmanSoftSonnette.disconnect();
    }
    /**
     * @brief Echec lors de la connexion au serveur
     */
    public void errorConnection()
    {
        byte[][] error = new byte[1][1];
        error[0][0] = 0;
        this.dispatcher.dispatch(error);
    }
    /**
     * @brief Envoie l'heure de l'AOP vers SoftSonnette
     */
    public void setCurrentTime(byte[] clock)
    {
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeTime(this.protocol.CMD_ID_CLOCK, this.protocol.NB_ARG_CLOCK, clock));
    }
    /**
     * @brief Demande la liste des employés.
     */
    public void askEmployeeList()
    {
        //Crée la trame pour le message puis l'envoie.
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_ASK_EMPLOYEE_LIST, this.protocol.NB_ARG_ASK_EMPLOYEE_LIST, null));
    }
    /**
     * @brief Ajoute un employé et l'envoie à softSonnette.
     */
    public void addEmployee(String name, String firstName, byte[] picture, int role, byte[] workingHours)
    {
        String workingHoursString = new String(Arrays.copyOfRange(workingHours, 0, workingHours.length));
        String pictureString = null;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O)
        {
            pictureString = Base64.getEncoder().encodeToString(picture);
        }
        byte[] data = String.format("%s|%s|%s|%d|%s", name, firstName, pictureString, role, workingHoursString).getBytes();
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_ADD_EMPLOYEE, this.protocol.NB_ARG_ADD_EMPLOYEE, data));
    }
    /**
     * @brief Demande l'activation ou la désactivation du stream de la vidéo.
     * @param enable état de la demande du stream.
     */
    public void subscribeToVideoStream(boolean enable)
    {
        int res;
        if (enable) res =1;
        else res = 0;
        byte[] data = String.valueOf(res).getBytes();
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_STREAM, this.protocol.NB_ARG_STREAM, data));
    }
    /**
     * @brief Demande l'ouverture de la porte.
     */
    public void askOpenDoor()
    {
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_ASK_OPEN_DOOR, this.protocol.NB_ARG_ASK_OPEN_DOOR, null));
    }
    /**
     * @brief Demande l'état de la porte.
     */
    public void askDoorState()
    {
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_ASK_DOOR_STATE, this.protocol.NB_ARG_ASK_DOOR_STATE, null));
    }
    /**
     * @brief Supprime un employé.
     */
    public void deleteEmployee(int employeeID)
    {
        byte[] data = String.valueOf(employeeID).getBytes();
        this.postmanSoftSonnette.sendMsg(this.protocol.encodeMessage(this.protocol.CMD_ID_DELETE_EMPLOYEE, this.protocol.NB_ARG_DELETE_EMPLOYEE, data));
    }
    /**
     * @brief Pattern observateur signalant l'établissement de la connexion.
     */
    @Override
    public void onConnectionEstablished()
    {
        isConnected = true;
        run();
        Log.d(TAG, "Valeur de pass dans onConnectionEtablished : " + this.pass);
        askCheckPass(pass);
    }
    /**
     * @brief Pattern observateur signalant la perte de la connexion.
     */
    @Override
    public void onConnectionLost()
    {
        isConnected = false;
        Log.e(TAG, "onConnectionLost");
        endConnection();
        errorConnection();
    }
    /**
     * @brief Pattern observateur signalant l'échec de la connexion.
     */
    @Override
    public void onConnectionFailed()
    {
        Log.d(TAG, "Echec lors de la connection avec le serveur dans le onConnectionFailed");
        errorConnection();
    }
}
