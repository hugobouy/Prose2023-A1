/**
 * @file 	Connection.java
 * @brief	Classe faisant la connexion avec le serveur
 * @author 	CASSAR Bastien
 * @date 	15/04/2023
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

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class PostmanSoftSonnette
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief variable pour le Pattern observateur de la connexion TCP/IP.
     */
    private final List<ConnectionObserver> observers = new ArrayList<>();
    /**
     * @brief Variable de la socket de connexion.
     */
    private Socket mySocket;
    /**
     * @brief Initialise le nombre de thread stocké dans la variable executorService.
     */
    private final ExecutorService executorService = Executors.newFixedThreadPool(1);

    /*
     * FUNCTIONS
     */
    /**
     * @brief Création d'une interface du Pattern observateur.
     */
    public interface ConnectionObserver
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
    public void addObserver(ConnectionObserver observer)
    {
        observers.add(observer);
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion est établie.
     */
    private void notifyConnectionEstablished()
    {
        for (ConnectionObserver observer : observers)
        {
            observer.onConnectionEstablished();
        }
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion a échoué.
     */
    private void notifyConnectionFailed()
    {
        for (ConnectionObserver observer : observers)
        {
            observer.onConnectionFailed();
        }
    }
    /**
     * @brief Appelle toutes les classes implémentant le Pattern observateur pour leurs notifié que la connexion est perdu.
     */
    private void notifyConnectionLost()
    {
        for (ConnectionObserver observer : observers)
        {
            observer.onConnectionLost();
        }
    }
    /**
     * @brief Connecte la socket.
     * @param IP [in] adresse ip du serveur.
     * @param PORT [in] port du serveur.
     * @param timeout [out] TAC (Temps Attente Connexion)
     */
    public void connectToServer(String IP, int PORT, int timeout)
    {
        Runnable runnable = () ->
        {
            try
            {
                PostmanSoftSonnette.this.mySocket = new Socket();
                InetSocketAddress inetSocketAddress = new InetSocketAddress(InetAddress.getByName(IP),PORT);
                PostmanSoftSonnette.this.mySocket.connect(inetSocketAddress, timeout);
                notifyConnectionEstablished();
            }
            catch (IOException e)
            {
                Log.i(TAG, "Echec lors de la connexion (" + e.getMessage() + ")");
                notifyConnectionFailed();
            }
        };
        executorService.execute(runnable);
    }
    /**
     * @brief Envoie un message via la socket.
     * @param msg [in] Message envoyé sur le socket
     */
    public void sendMsg(byte[] msg)
    {
        Runnable runnable = () ->
        {
            try
            {
                //Ecrit sur la socket le message.
                PostmanSoftSonnette.this.mySocket.getOutputStream().write(msg);
                //Force l'écriture des données en attente.
                PostmanSoftSonnette.this.mySocket.getOutputStream().flush();
            }
            catch (IOException e)
            {
                notifyConnectionLost();
                Log.i(TAG, "Echec lors de l'envoie d'un message (" + e.getMessage() + ")");
            }
        };
        executorService.execute(runnable);
    }
    /**
     * @brief Lis un message via la socket.
     * @param size [in] Taille du mesage reçu
     * @return byte[] Message reçu
     */
    public byte[] readMsg(short size) throws IOException
    {
        try
        {
            byte[] buffer = new byte[size];
            int bytesRead;
            bytesRead = PostmanSoftSonnette.this.mySocket.getInputStream().read(buffer, 0, size);
            if (bytesRead > -1)
            {
                if (bytesRead > 0)
                {
                    return buffer;
                }
                else
                {
                    throw new IOException("Zero bytes en entrés.");
                }
            }
            else
            {
                throw new IOException("Fin du flux.");
            }
        }
        catch (IOException e)
        {
            notifyConnectionLost();
            throw e;
        }
    }
    /**
     * @brief Ferme la socket
     */
    public void disconnect()
    {
        try
        {
            PostmanSoftSonnette.this.mySocket.close();
        }
        catch (IOException e)
        {
            Log.i(TAG, "Déconnexion de la socket (" + e.getMessage() + ")");
        }
    }
}

