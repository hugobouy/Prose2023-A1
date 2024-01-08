/**
 * @file 	CacheCameraman.java
 * @brief	Classe permettant de traiter un flux vidéo Mpeg4 et de le reconstruire
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

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;

import androidx.annotation.NonNull;

import com.prose.a1.aop.communication.Communication;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.fragment.FragmentVideo;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class CacheCameraman implements PostmanVideo.ConnectionObserverVideo, TextureView.SurfaceTextureListener
{

    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief Variable définissant l'état de la du flux UDP avec la socket.
     */
    private boolean onConnected = false;
    /**
     * @brief tag pour les debbug.
     */
    private static final String TAG = "TAG";
    /**
     * @brief Variable définissant la longueur maximum d'un frame Mpeg4.
     */
    private final int lengthFrameMax = 65535;

    /**
     * @brief Variable définissant la surface utilisée pour la mise à jour de la vidéo.
     */
    private Surface mSurface;

    /**
     * @brief Variable définissant s'il s'agit du 1er frame à décoder.
     */
    private boolean FirstFrame = true;

    /**
     * @brief Variable définissant s'il s'agit du 1er frame qui commence par une START_FRAME, et qui est à décoder.
     */
    private boolean FirstFrameStart = false;

    /**
     * @brief Variable définissant le temps minimal entre le décodage de 2 frames.
     */
    private static long presentationTime = 0;

    /**
     * @brief Variable définissant la composition des bytes pour reconnaitre START_FRAME pour une vidéo Mpeg4.
     */
    private static final byte[] START_FRAME_BYTES = {0x00, 0x00, 0x01, (byte) 0xb0};
    /**
     * @brief Variable définissant la composition des bytes pour reconnaitre KEY_FRAME pour une vidéo Mpeg4.
     */
    private static final byte[] KEY_FRAME_BYTES = {0x00, 0x00, 0x01, (byte) 0xb6};
    /**
     * @brief Variable définissant le mediaCodec, le décodeur, permettant la recomposition de la vidéo à partir de frames.
     */
    private MediaCodec mediaCodec;

    /**
     * @brief Variable définissant les informations utiles à la configuration d'un mediaCodec.
     */
    MediaCodec.BufferInfo bufferInfo;

    /**
     * @brief Variable définissant la class gérant la lecture du flux UDP.
     */
    private final PostmanVideo PostmanVideo;
    /**
     * @brief Variable définissant le buffer (tableau de byte) contenant les données reçues dans un paquet UDP.
     */
    public byte[] bufferData = new byte[lengthFrameMax];
    /**
     * @brief Variable définissant le buffer (tableau de byte) contenant les données prêtes à être envoyées au MediaCodec.
     */
    private byte[] bufferCodec = new byte[lengthFrameMax];

    /**
     * @brief Variable définissant le fragment de la vidéo.
     */
    private final FragmentVideo parentFragment;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     * @param textureView Texture de la class, où se mettra à jour la vidéo.
     * @param parentFragment Fragment où se fera la mise à jour de la vidéo.
     */
    public CacheCameraman(@NonNull TextureView textureView, FragmentVideo parentFragment)
    {
        this.PostmanVideo = new PostmanVideo();
        this.PostmanVideo.addObserverVideo(this);
        this.parentFragment = parentFragment;
        textureView.setSurfaceTextureListener(this);
    }
    /**
     * @brief Pattern observateur signalant l'établissement de la connexion.
     */
    @Override
    public void onConnectionEstablished()
    {
        onConnected = true;
        Communication.INSTANCE.subscribeToVideoStream(true);
        Log.i(TAG, "$$$ Connecté $$$");
    }
    /**
     * @brief Pattern observateur signalant le perte de la connexion suite à un timeout.
     */
    @Override
    public void onConnectionLost()
    {
        Log.i(TAG, "$$$ Connexion perdu $$$");
        Integer errorConnectionValue = ConnectionManager.INSTANCE.getErrorConnection().getValue();
        if (errorConnectionValue != null && errorConnectionValue != 7)
        {
            parentFragment.showPopup();
            onConnected = false;
            Communication.INSTANCE.subscribeToVideoStream(false);
        }
        PostmanVideo.disconnectSocket();
    }
    /**
     * @brief Pattern observateur signalant la perte du flux de la vidéo.
     */
    @Override
    public void onConnectionFailed()
    {
        Log.i(TAG, "$$$ Connexion failed $$$");
        onConnected = false;
        PostmanVideo.disconnectSocket();
        Communication.INSTANCE.subscribeToVideoStream(false);
    }
    /**
     * @brief Méthode d'initialisation du MediaCodec.
     */
    private void initializeMediaCodec()
    {
        try
        {
            // Initialisation du MediaCodec pour le décodage vidéo
            String VIDEO_FORMAT = "video/mp4v-es";
            mediaCodec = MediaCodec.createDecoderByType(VIDEO_FORMAT);
            bufferInfo = new MediaCodec.BufferInfo();
            int VIDEO_WIDTH = 640;
            int VIDEO_HEIGHT = 480;
            MediaFormat format = MediaFormat.createVideoFormat(VIDEO_FORMAT, VIDEO_WIDTH, VIDEO_HEIGHT);
            mediaCodec.configure(format, mSurface, null, 0);
            mediaCodec.start();
            Log.d(TAG, "$$$ initializeMediaCodec & Start $$$");

        }
        catch (IOException e)
        {
            Log.e(TAG, "$$$ FAIL : initializeMediaCodec $$$");
        }
    }
    /**
     * @brief Méthode permettant la mise à jour de la vidéo.
     */
    public void run()
    {
        //Lance le thread faisant la lecture des messages venant du serveur.
        Thread thread = new Thread(() ->
        {
            try
            {
                int indexBufferCodec = 0;
                while (onConnected)
                {
                    bufferData = new byte[lengthFrameMax];

                    int sizeMsg = PostmanVideo.readMsg(bufferData);
                    //Log.d(TAG, " --> sizeMsg de taille : "+ sizeMsg + " et taille indexBufferCodec : "+ indexBufferCodec + "  et BufferData = " +Arrays.toString(bufferData));

                    int key = bufferData[3];
                    if(START_FRAME_BYTES[0] == bufferData[0] && START_FRAME_BYTES[1] == bufferData[1] && START_FRAME_BYTES[2] == bufferData[2]
                            && (START_FRAME_BYTES[3] == key || KEY_FRAME_BYTES[3] == key))
                    {
                        //Log.d(TAG, " * Détecte Frame *    Contenu bufferCodec : " + Arrays.toString(bufferCodec));
                        stream(bufferCodec);
                        indexBufferCodec = 0;
                        bufferCodec = new byte[lengthFrameMax];

                        // Recherche d'une KEY_FRAME dans le paquet suivant une START_FRAME
                        if(key == START_FRAME_BYTES[3])
                        {
                            int startIndex = -1;
                            for (int i = 3; i < bufferData.length - KEY_FRAME_BYTES.length; i++)
                            {
                                boolean found = true;
                                for (int j = 0; j < KEY_FRAME_BYTES.length; j++)
                                {
                                    if (bufferData[i + j] != KEY_FRAME_BYTES[j])
                                    {
                                        found = false;
                                        break;
                                    }
                                }
                                if (found)
                                {
                                    startIndex = i;
                                    break;
                                }
                            }
                            if(startIndex != -1)
                            {
                                byte[][] result = decomposeBuffer(bufferData, startIndex);
                                stream(result[0]);
                                sizeMsg -= result[0].length;
                                bufferData = Arrays.copyOfRange(result[1], 0, result[1].length);
                            }
                        }
                    }
                    // Copie des données reçues dans le bufferCodec
                    System.arraycopy(bufferData, 0, bufferCodec, indexBufferCodec, sizeMsg);
                    indexBufferCodec += sizeMsg;
                }
            }
            catch (Exception e)
            {
                Log.e(TAG, "Echec lors de la lecture du message (" + e.getMessage() + ")");
            }
        });
        thread.start();
    }
    /**
     * @brief Méthode permettant l'affichage du frame détecté, et la gestion du 1er frame.
     * @param BufferCodec Tableau de bytes contenant un frame détecté (et sisolé).
     */
    private void stream(@NonNull byte[] BufferCodec)
    {
        if(BufferCodec[3] == 0 && BufferCodec[4] == 0 && FirstFrame)
        {
            //Log.i(TAG, "$$$ Début du STREAM $$$");
            FirstFrame = false;
        }
        else if(BufferCodec[3] == START_FRAME_BYTES[3])
        {
            //Log.d(TAG, "--> STREAM d'un START_FRAME --- BufferCodec de taille : "+ BufferCodec.length + "  contenu :  "+ Arrays.toString(BufferCodec));
            callback(BufferCodec);
            FirstFrameStart = true;
        } else if (BufferCodec[3] == KEY_FRAME_BYTES[3] && FirstFrameStart)
        {
            //Log.d(TAG, "--> STREAM d'un KEY_FRAME   --- BufferCodec de taille : "+ BufferCodec.length + "  contenu :  "+ Arrays.toString(BufferCodec));
            callback(BufferCodec);
        }
        else
        {
            Log.e(TAG, "$$$ STREAM d'un FRAME inconnu $$$ BufferCodec de taille : "+ BufferCodec.length + "  contenu :  "+ Arrays.toString(BufferCodec));
        }
    }
    /**
     * @brief Fonction permettant de décomposer un tableau de bytes en 2 tableau de bytes, à partir d'un index.
     * @param buffer Tableau de bytes à décomposer.
     * @param index Index qui indique la décomposition du tableau de bytes.
     * @return Retour de 2 tableaux de bytes.
     * @throws IllegalArgumentException Si l'index est hors limites (inférieur à 0 ou supérieur à la taille du buffer).
     */
    public byte[][] decomposeBuffer(@NonNull byte[] buffer, int index) throws IllegalArgumentException
    {
        if (index < 0 || index > buffer.length) {
            throw new IllegalArgumentException("L'index est hors limites");
        }
        byte[] buffer1 = new byte[index];
        byte[] buffer2 = new byte[buffer.length - index];

        System.arraycopy(buffer, 0, buffer1, 0, index);
        System.arraycopy(buffer, index, buffer2, 0, buffer.length - index);

        return new byte[][] { buffer1, buffer2 };
    }
    /**
     * @brief Méthode appelé pour envoyé le frame au MediaCodec.
     * @param frameData Frame prêt à être traitée par le MediaCodec.
     */
    private void callback(byte[] frameData)
    {
        // Récupération d'un tampon d'entrée pour le décodage
        int inputBufferId = mediaCodec.dequeueInputBuffer(0);
        if (inputBufferId >= 0) {
            ByteBuffer inputBuffer = mediaCodec.getInputBuffer(inputBufferId);
            if (inputBuffer != null) {
                // Remplie l'inputBuffer avec la frameData
                inputBuffer.put(frameData, 0, frameData.length);
                // Soumet le tampon d'entrée au MediaCodec pour traitement
                presentationTime += 10;
                mediaCodec.queueInputBuffer(inputBufferId, 0, frameData.length, presentationTime, 0);
            }
        }
        // Libération des tampons de sortie du MediaCodec
        int outputBufferId;
        while ((outputBufferId = mediaCodec.dequeueOutputBuffer(bufferInfo, 0)) >= 0) {
            mediaCodec.releaseOutputBuffer(outputBufferId, true);
        }
    }
    /**
     * @brief Méthode appelée lorsque la texture de surface est disponible.
     * @param surfaceTexture La texture de surface disponible.
     * @param width La largeur de la texture de surface.
     * @param height La hauteur de la texture de surface.
     */
    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int width, int height)
    {
        // Obtenir la surface associée à la TextureView
        mSurface = new Surface(surfaceTexture);

        try {
            initializeMediaCodec();

            if(onConnected)
            {
                run();
            }
            else
            {
                Log.e(TAG, "$$$ onSurfaceTextureAvailable mais pas connecté $$$");
            }
        } catch (IllegalStateException e) {
            Log.e(TAG, "$$$ FAIL : onSurfaceTextureAvailable $$$");
        }
    }
    /**
     * @brief Méthode appelée lorsque la taille de la texture de surface est modifiée.
     * @param surfaceTexture La texture de surface.
     * @param width Nouvelle largeur de la texture.
     * @param height Nouvelle hauteur de la texture.
     */
    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int width, int height)
    {

    }
    /**
     * @brief Méthode appelée lorsque la texture de surface est détruite.
     * @param surfaceTexture La texture de surface.
     * @return Retourne false pour indiquer que la texture de surface n'est pas détruite immédiatement.
     */
    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture)
    {
        PostmanVideo.disconnectSocket();
        return false;
    }
    /**
     * @brief Méthode appelée lorsque la texture de surface est mise à jour.
     * @param surfaceTexture La texture de surface.
     */
    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture)
    {

    }
}

