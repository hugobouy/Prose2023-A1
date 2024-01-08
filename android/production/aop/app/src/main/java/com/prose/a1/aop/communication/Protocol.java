/**
 * @file 	DecodeProtocol.java
 * @brief	Classe permettant le decodage de la trame
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

import static java.lang.Byte.BYTES;

import java.nio.ByteBuffer;
import java.util.StringTokenizer;

public class Protocol
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur pour supprimer un employé.
     */
    public final int CMD_ID_DELETE_EMPLOYEE = 9;
    /**
     * @brief nombre d'argument pour supprimer un employé.
     */
    public final int NB_ARG_DELETE_EMPLOYEE = 1;
    /**
     * @brief valeur de l'id de l'état de la porte.
     */
    public final int CMD_ID_ASK_DOOR_STATE = 6;
    /**
     * @brief nombre d'argument pour l'état de la porte.
     */
    public final int NB_ARG_ASK_DOOR_STATE= 0;
    /**
     * @brief valeur de l'id de l'ouverture de la porte.
     */
    public final int CMD_ID_ASK_OPEN_DOOR = 5;
    /**
     * @brief nombre d'argument pour l'ouverture de la porte
     */
    public final int NB_ARG_ASK_OPEN_DOOR= 0;
    /**
     * @brief valeur de l'id pour ajouter un employé.
     */
    public final int CMD_ID_ADD_EMPLOYEE = 8;
    /**
     * @brief nombre d'argument pour ajouter un employé.
     */
    public final int NB_ARG_ADD_EMPLOYEE = 5;
    /**
     * @brief valeur de l'id de la commande du stream
     */
    public final int CMD_ID_STREAM = 1;
    /**
     * @brief nombre d'argument lors du stream
     */
    public final int NB_ARG_STREAM = 1;
    /**
     * @brief valeur de l'id de la commande de clock
     */
    public final int CMD_ID_CLOCK = 4;
    /**
     * @brief nombre d'argument lors de la clock
     */
    public final int NB_ARG_CLOCK = 1;
    /**
     * @brief valeur de l'id de la commande de connexion
     */
    public final int CMD_ID_CONNECTION = 2;
    /**
     * @brief nombre d'argument lors de la connexion
     */
    public final int NB_ARG_CONNECTION = 1;
    /**
     * @brief valeur de l'id pour demander la liste des employées.
     */
    public final int CMD_ID_ASK_EMPLOYEE_LIST = 10;
    /**
     * @brief nombre d'argument pour demander la liste des employées.
     */
    public final int NB_ARG_ASK_EMPLOYEE_LIST = 0;
    /**
     * @brief identifiant de la commande
     */
    private int cmdId;
    /**
     * @brief nombre d'argument
     */
    private int nbArgs;
    /**
     * @brief longueur de la donné a envoyé
     */
    private int dataLength;
    /**
     * @brief Valeur de la trame finale
     */
    private byte[] finalTrame;

    /*
     * FUNCTIONS
     */
    /**
     * @brief crée la trame pour la clock
     * @param cmdId [in] identifiant de la commande
     * @param nbArgs [in] nombre d'argument
     * @param data [in] donnée a envoyé
     */
    public byte[] encodeTime(int cmdId, int nbArgs, byte[] data)
    {
        this.cmdId = cmdId;
        this.nbArgs = nbArgs;
        this.dataLength = data.length;
        //Log.e(TAG, "Longueur de la trame : " + dataLength);
        int size = this.dataLength + this.nbArgs*(Short.BYTES);
        ByteBuffer protocolBuffer = ByteBuffer.allocate(size);
        protocolBuffer.putShort(0, (byte) dataLength);
        for (int j = 0; j < dataLength; j++)
        {
            //Entre les différentes valeurs de l'argument
            protocolBuffer.put((Short.BYTES) + j, data[j]);
        }
        this.dataLength = size;
        this.finalTrame = protocolBuffer.array();
        buildTrame();

        return this.finalTrame;
    }
    /**
     * @brief crée la trame
     * @param cmdId [in] identifiant de la commande
     * @param nbArgs [in] nombre d'argument
     * @param data [in] donnée a envoyé
     */
    public byte[] encodeMessage(int cmdId, int nbArgs, byte[] data)
    {
        this.cmdId = cmdId;
        this.nbArgs = nbArgs;
        //condition pour savoir s'il faut construire une trame pour la donnée
        if (data == null)
        {
            //Log.d(TAG, "begin null getProtocol : ");
            this.dataLength = 0;
            buildRawTrame();
        }
        else
        {
            this.dataLength = data.length;
            buildData(data);
            buildTrame();
        }
        return this.finalTrame;
    }
    /**
     * @brief Construit la trame de la donnée
     * @param data [in] Valeur de la donnée
     */
    private void buildData(byte[] data)
    {
        //Sépare les arguments entre eux
        int nbSeparator = (this.nbArgs - 1)*(BYTES);
        if(this.nbArgs == 0) nbSeparator = 0;
        //Détermine la taille de l'argument
        int size = this.dataLength + this.nbArgs*(Short.BYTES)- nbSeparator;

        String str = new String(data);
        //Permet de délimiter les arguments grâce au caractère spécial.
        StringTokenizer tokenizer = new StringTokenizer(str, "|");
        String currentArg = tokenizer.nextToken();

        //Log.d(TAG, "Valeur de currentArg: "+ currentArg);

        //Initialise avec la bonne taille la valeur qui sera retourner
        ByteBuffer protocolBuffer = ByteBuffer.allocate(size);
        int padding = 0;

        for(int i = 0; i< this.nbArgs; i++)
        {
            assert currentArg != null;
            short len = (short) currentArg.length();

            //Construit la trame, 1 octet la taille
            protocolBuffer.putShort(padding, len);
            for (int j = 0; j < len; j++)
            {
                //Entre les différentes valeurs de l'argument
                protocolBuffer.put(padding + (Short.BYTES) + j, (byte) currentArg.charAt(j));
            }
            //Valeur de fin d'argument
            padding += 2 * (BYTES) + len;

            //S'il y a plus d'un argument
            if (tokenizer.hasMoreTokens())
            {
                currentArg = tokenizer.nextToken();
            } else {
                currentArg = null;
            }
        }
        this.dataLength = size;
        this.finalTrame = protocolBuffer.array();
    }
    /**
     * @brief Construction de la trame pour une valeur de donnée nulle
     */
    private void buildRawTrame()
    {
        //Taille du protocole
        short size = (BYTES) + (BYTES) + (Short.BYTES);

        //Log.d(TAG, "frameSize : "+ size);

        //Construction du protocole
        ByteBuffer protocolBuffer = ByteBuffer.allocate(size);
        protocolBuffer.putShort(0, (byte) size);
        protocolBuffer.put((Short.BYTES), (byte) cmdId);
        protocolBuffer.put((BYTES) + (Short.BYTES), (byte) nbArgs);

        this.finalTrame = protocolBuffer.array();
    }
    /**
     * @brief Construction de la trame pour une valeur de donnée non nulle
     */
    private void buildTrame()
    {
        //Taille du protocole
        short size = (short) (2*(BYTES) +  this.dataLength + (Short.BYTES));

        //Log.d(TAG, "frameSize : "+ size);

        //Construction du protocole
        ByteBuffer protocolBuffer = ByteBuffer.allocate(size);
        protocolBuffer.putShort(0, size);
        protocolBuffer.put((Short.BYTES), (byte) this.cmdId);
        protocolBuffer.put((BYTES) + (Short.BYTES), (byte) this.nbArgs);
        protocolBuffer.put((BYTES) + (BYTES) + (Short.BYTES), (byte) this.nbArgs);
        for (int j = 0; j < this.dataLength; j++)
        {
            //Mise en place de la trame construite avant dans le protocole
            protocolBuffer.put((BYTES) + (BYTES) + (Short.BYTES) + j, this.finalTrame[j]);
        }

        this.finalTrame = protocolBuffer.array();
    }
    /**
     * @param messageDecode [in] Message a décodé.
     * @brief Décode la trame reçu.
     */
    public byte[][] decodeMessage(byte[] frameSize, byte[] messageDecode)
    {
        //Initialisation de variable permettant de décoder la trame.
        boolean firstEntry = true;
        byte cmdId;
        byte nbArgs;
        short frameLen;
        byte argIndex = 3;
        byte argIndexData = 0;
        short currentArgIndex = 0;
        int valSize = 0;

        frameLen = (byte) getIntLength(frameSize[1], frameSize[0]);
        cmdId = messageDecode[0];
        nbArgs = messageDecode[1];

        byte[][] args = new byte[nbArgs+3][];
        short[] dataLen = new short[nbArgs];
        args[0] = new byte[]{cmdId};
        args[1] = new byte[]{nbArgs};

        for (int i = 2; i<frameLen-2; i++)
        {
            if (nbArgs == 1 && firstEntry)
            {
                dataLen[argIndexData] = (byte) getIntLength(messageDecode[i+1], messageDecode[i]);
                args[argIndex] = new byte[dataLen[argIndexData]];
                //Log.d(TAG, "Longueur de l'argument n° " + argIndex + " : " + dataLen[argIndexData]);
                i+=2;
                firstEntry = false;
            }
            else if(nbArgs !=1 && i == 2+valSize || firstEntry)
            {
                if (!firstEntry)
                {
                    currentArgIndex = 0;
                    argIndex++;
                    firstEntry = true;
                }
                dataLen[argIndexData] = (byte) getIntLength(messageDecode[i+1], messageDecode[i]);
                args[argIndex] = new byte[dataLen[argIndexData]];
                //Log.d(TAG, "Longueur de l'argument n° " + argIndex + " : " + dataLen[argIndexData]);
                i += 2;
                valSize += dataLen[argIndexData]+2;
                firstEntry = false;
            }
            args[argIndex][currentArgIndex] = messageDecode[i];
            currentArgIndex++;
            //Log.d(TAG, "Valeur de messageDecode : " + messageDecode[i]);
        }

        //Log.d(TAG, "Valeur finale  ");
        args[2] = new byte[]{(byte) dataLen[argIndexData]};
        //Initialise une variable avec les différents éléments de la trame et la retourne.
        return args;
    }
    /**
     * @brief Permet de retourner la valeur entière de 2 octets
     * @param highByte [in] bit de poid fort
     * @param lowByte [in] bit de poid faible
     */
    private int getIntLength(byte lowByte, byte highByte)
    {
        return ((int)highByte << 8) + ((int)lowByte);
    }
}
