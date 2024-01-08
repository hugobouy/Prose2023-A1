/**
 * @file 	PostmanVideo.java
 * @brief	Classe permettant de réceptionner le flux UDP
 * @author 	Mathis Moulin
 * @date 	06/06/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

package com.prose.a1.aop.model.video;

import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class PostmanVideo
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief Tag pour utiles aux déboguages.
     */
    private static final String TAG = "TAG";
    /**
     * @brief Valeur du port de la socket de lecture du flux vidéo
     */
    private final int IP_PORT = 5000;
    /**
     * @brief ExecutorService utilisé pour exécuter les tâches en arrière-plan.
     */
    private final ExecutorService executorService = Executors.newFixedThreadPool(1);
    /**
     * @brief Liste des observateurs de connexion.
     */
    private final List<ConnectionObserverVideo> observers = new ArrayList<>();
    /**
     * @brief Durée d'attente avant la fin du stream en millisecondes.
     */
    private static final int TIMEOUT_DURATION = 7000;
    /**
     * @brief Socket Datagram utilisée pour la communication.
     */
    private DatagramSocket mySocket;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    public PostmanVideo()
    {
        connectToStreamer(IP_PORT);
    }
    /**
     * @brief Création d'une interface du Pattern observateur Vidéo.
     */
    public interface ConnectionObserverVideo
    {
        /**
         * @brief Méthode appelée lorsque la connexion est établie
         */
        void onConnectionEstablished();
        /**
         * @brief Méthode appelée lorsque la connexion est perdue.
         */
        void onConnectionLost();
        /**
         * @brief Méthode appelée lorsque la connexion a échoué.
         */
        void onConnectionFailed();
    }
    /**
     * @brief Permet d'ajouter les classes qui implémentent cette interface.
     * @param observer [in] Lie une instance avec le pattern observateur
     */
    public void addObserverVideo(ConnectionObserverVideo observer)
    {
        observers.add(observer);
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion est établie.
     */
    private void notifyConnectionEstablished()
    {
        for (ConnectionObserverVideo observer : observers)
        {
            observer.onConnectionEstablished();
        }
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion a échoué.
     */
    private void notifyConnectionFailed()
    {
        for (ConnectionObserverVideo observer : observers)
        {
            observer.onConnectionFailed();
        }
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion est perdu.
     */
    private void notifyConnectionLost()
    {
        for (ConnectionObserverVideo observer : observers)
        {
            observer.onConnectionLost();
        }
    }
    /**
     * @brief Méthode qui établit une connexion avec le streamer à l'aide du port spécifié.
     * @param PORT Le numéro de port à utiliser pour la connexion.
     */
    public void connectToStreamer(int PORT)
    {
        Runnable runnable = () ->
        {
            try
            {
                PostmanVideo.this.mySocket = new DatagramSocket(PORT);
                notifyConnectionEstablished();
                //Log.i(TAG, "notifyConnectionEstablished");
            }
            catch (IOException e)
            {
                Log.e(TAG, "Echec lors de la connexion (" + e.getMessage() + ")");
                notifyConnectionFailed();
            }
        };
        executorService.execute(runnable);
    }
    /**
     * @brief Fonction qui lit un message UDP et retourne la taille du paquet.
     * @param message Le tableau de bytes dans lequel le message sera stocké.
     * @return La taille du paquet reçu.
     * @throws IOException En cas d'erreur lors de la réception des données.
     */
    public int readMsg(byte[] message) throws IOException
    {
        int sizePacket = 0;
        try
        {
            DatagramPacket receivePacket = new DatagramPacket(message, message.length);
            // Détection de fin de stream
            try
            {
                // Recevoir les données de la frame via DatagramSocket avec un délai d'attente
                mySocket.setSoTimeout(TIMEOUT_DURATION);
                mySocket.receive(receivePacket);
                sizePacket = receivePacket.getLength();
            }
            catch (SocketTimeoutException e)
            {
                // Détection de la fin du stream (timeout atteint)
                Log.e(TAG, "$$$ FAIL : Fin du stream (timeout atteint) $$$");
                notifyConnectionLost();
            }
        }
        catch (IOException e)
        {
            Log.e(TAG, "readMsg : $$$ Erreur lors de la réception des données en UDP $$$");
            notifyConnectionFailed();
            throw e;
        }
        return sizePacket;
    }
    /**
     * @brief Méthode qui ferme la socket de communication.
     */
    public void disconnectSocket()
    {
        if(PostmanVideo.this.mySocket != null)
        {
            PostmanVideo.this.mySocket.close();
        }
    }
}


