/**
 * @file 	FragmentVideo.java
 * @brief	Classe permettant d'afficher l'écran de la vidéo
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

package com.prose.a1.aop.view.fragment;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;

import com.prose.a1.aop.R;
import com.prose.a1.aop.model.video.CacheCameraman;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;

public class FragmentVideo extends Fragment
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief instance du viewModel de connexion
     */
    ViewModelConnection viewModelConnection;
    /**
     * @brief création de la variable builder pour la Pop-Up Erreur de Connexion.
     */
    AlertDialog.Builder builder;
    /**
     * @brief vue utilisé pour le FragmentAddEmployee.
     */
    View view;
    /**
     * @brief création de la variable permettant la gestion de la mise à jour de la vidéo.
     */
    private CacheCameraman cacheCameraman;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    public FragmentVideo()
    {
        // Required empty public constructor
        Log.d(TAG, "$$$ Fragment Video $$$");
    }
    /**
     * @brief Permet de créer la vue du fragment avec les éléments du .xml.
     * @param inflater [in] créer une instance de view à partir du fichier xml du calendrier.
     * @param container [in] Conteneur parent dans lequel la vue sera créer.
     * @param savedInstanceState [in] contient les données de l'état précédent la vue.
     * @return Retourne la vue créée
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {

        // Obtenez le contexte de l'activité parente
        view = inflater.inflate(R.layout.fragment_video, container, false);
        TextureView mTextureView = view.findViewById(R.id.textureView);

        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(this).get(ViewModelConnection.class);
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
        {
            if (value == 7)
            {
                Log.d(TAG, "valeur CreateView : "+ ConnectionManager.INSTANCE.getErrorConnection().getValue());
                errorConnectionVideoPopUp(view);
            }
        });
        // Créez une instance de CacheVideo et passez la référence de la TextureView
        cacheCameraman = new CacheCameraman(mTextureView, this);

        // Récupérer le bouton "buttonReturnHome" depuis la vue
        Button buttonReturnHome = view.findViewById(R.id.buttonReturnHome);

        // Gérer le clic sur le bouton "buttonReturnHome"
        buttonReturnHome.setOnClickListener(v -> {
            // Naviguer vers le FragmentHome
            cacheCameraman.onConnectionFailed();
            Navigation.findNavController(v).navigate(R.id.fragmentHome);
        });

        return view;
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionVideoPopUp(View displayConnection)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = 1;
                builder = new AlertDialog.Builder(requireActivity());
                builder.setMessage(R.string.PopUpErrorConnection);
                builder.setPositiveButton(R.string.BoutonReturn, (dialog, which) ->
                {
                    // Action à effectuer lorsque le bouton OK est cliqué
                    dialog.dismiss(); // Fermer la pop-up
                    ConnectionManager.INSTANCE.initErrorConnection();
                    Navigation.findNavController(displayConnection).navigate(R.id.fragmentConnection);
                });
                builder.setCancelable(false); // Empêcher la fermeture de la pop-up lors du clic en dehors d'elle
                AlertDialog dialog = builder.create();
                dialog.show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Affiche un popUp signalant la perte du flux vidéo UDP.
     */
    public void showPopup()
    {
        requireActivity().runOnUiThread(() ->
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setCancelable(false);
            builder.setTitle(R.string.VideoLost)
                    .setPositiveButton(R.string.BoutonReturn, (dialog, which) ->
                    {
                        // Action lors du clic sur le bouton 'Return'
                        Navigation.findNavController(view).navigate(R.id.fragmentHome);
                    })
                    .show();
        });
    }
}
