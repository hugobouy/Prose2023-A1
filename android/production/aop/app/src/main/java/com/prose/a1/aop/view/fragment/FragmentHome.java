/**
 * @file 	FragmentHome.java
 * @brief	Classe du fragment d'accueil
 * @author 	CASSAR Bastien
 * @date 	04/05/2023
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

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.prose.a1.aop.R;
import com.prose.a1.aop.communication.Communication;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;
import com.prose.a1.aop.view.viewModel.ViewModelEmployeeManager;

public class FragmentHome extends Fragment
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur fin de données persistantes
     */
    private static final int END_DATA = 1;
    /**
     * @brief valeur erreur de connexion dans l'accueil
     */
    private static final int ERREUR_CONNECTION = 2;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de la porte.
     */
    private static final int CNT_DOOR = 4;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran vidéo.
     */
    private static final int CNT_VIDEO = 7;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de la liste des employés.
     */
    private static final int CNT_LIST_EMPLOYEE = 5;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de connexion
     */
    private static final int CNT_CONNECTION = 1;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran Calendrier
     */
    private static final int CNT_CALENDAR = 3;
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief instance du viewModel de connexion
     */
    ViewModelConnection viewModelConnection;
    /**
     * @brief instance du viewModel de connexion
     */
    ViewModelEmployeeManager viewModelEmployeeManager;
    /**
     * @brief permet de créer les popUp.
     */
    AlertDialog.Builder builder;
    /**
     * @brief variable de la vue du fragment
     */
    View root;
    /**
     * @brief Bouton permettant d'accèder à l'écran calendrier.
     */
    ImageView imageCalendar;
    /**
     * @brief Bouton permettant d'accèder à l'écran de la liste des employés.
     */
    ImageView imageEmployee;
    /**
     * @brief Bouton permettant d'accèder à l'écran vidéo.
     */
    ImageView imageVideo;
    /**
     * @brief Bouton permettant d'accèder à l'écran du contrôle de la porte.
     */
    ImageView imagePorte;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Construct de la classe
     */
    public FragmentHome()
    {
        // Required empty public constructor
    }
    /**
     * @brief Permet de créer la vue du fragment avec les éléments du .xml.
     * @param inflater [in] créer une instance de view à partir du fichier xml du calendrier.
     * @param container [in] Conteneur parent dans lequel la vue sera créer.
     * @param savedInstanceState [in] contient les données de l'état précédent la vue.
     */
    @Override
    public View onCreateView( LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        root = inflater.inflate(R.layout.fragment_home, container, false);
        (root.findViewById(R.id.buttonQuit)).setOnClickListener(this::onClickQuit);
        ConnectionManager.INSTANCE.initPass();
        CacheEmployeeManager.INSTANCE.deleteCache();
        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(this).get(ViewModelConnection.class);
        viewModelEmployeeManager = new ViewModelProvider(this).get(ViewModelEmployeeManager.class);
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == ERREUR_CONNECTION)
                    {
                        Log.i(TAG, "condition valider home getErrorConnection : " + viewModelConnection.getErrorConnection().getValue());
                        viewModelConnection.getErrorConnection().postValue(0);
                        errorConnectionHomePopUp(root);
                    }
                });
        viewModelEmployeeManager.getScreenCalendar().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == END_DATA)
                    {
                        askScreenCalendar(root);
                    }
                });
        viewModelEmployeeManager.getScreenEmployeeList().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == END_DATA)
                    {
                        askScreenEmployeeList();
                    }
                });

        this.imageCalendar = root.findViewById(R.id.buttonCalendar);
        this.imageCalendar.setOnClickListener(this::onClickCalendar);

        this.imageVideo = root.findViewById(R.id.buttonVideo);
        this.imageVideo.setOnClickListener(this::onClickVideo);

        this.imagePorte = root.findViewById(R.id.buttonDoor);
        this.imagePorte.setOnClickListener(this::onClickDoor);

        this.imageEmployee = root.findViewById(R.id.buttonEmployee);
        this.imageEmployee.setOnClickListener(this::onClickEmployee);
        return root;
    }
    /**
     * Affiche l'écran du calendrier
     * @param displayScreenCalendar vue du fragment
     */
    private void askScreenCalendar(View displayScreenCalendar)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_CALENDAR;
                CacheEmployeeManager.INSTANCE.initAskEmployee();
                Navigation.findNavController(displayScreenCalendar).navigate(R.id.fragmentCalendar);
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * Affiche l'écran de la liste des employés
     */
    private void askScreenEmployeeList()
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_LIST_EMPLOYEE;
                CacheEmployeeManager.INSTANCE.initAskEmployee();
                Navigation.findNavController(this.root).navigate(R.id.fragmentListEmployee);
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionHomePopUp(View displayConnection)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_CONNECTION;
                ConnectionManager.INSTANCE.initErrorConnection();
                builder = new AlertDialog.Builder(requireActivity());
                builder.setMessage(R.string.PopUpErrorConnection);
                builder.setPositiveButton(R.string.BoutonReturn, (dialog, which) ->
                {
                    // Action à effectuer lorsque le bouton OK est cliqué
                    dialog.dismiss(); // Fermer la pop-up
                    Navigation.findNavController(displayConnection).navigate(R.id.fragmentConnection);
                });
                builder.setCancelable(false); // Empêcher la fermeture de la pop-up lors du clic en dehors d'elle
                AlertDialog dialog = builder.create();
                dialog.show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Navigue vers un autre fragment lorsque le bouton Quitter est appuyé.
     * @param v [in] Permet de déclencher l'événement de click sur le bouton quitter.
     */
    public void onClickQuit(View v)
    {
        if (v.getId() == R.id.buttonQuit)
        {
            Communication.INSTANCE.endConnection();
            getActivity().finish();
            System.exit(0);
        }
    }
    /**
     * @brief Navigue vers un autre fragment lorsque l'image Calendrier est appuyé.
     * @param v [in] Permet de déclencher l'événement de click sur l'image Calendrier.
     */
    public void onClickCalendar(View v)
    {
        if (v.getId() == R.id.buttonCalendar)
        {

            Communication.INSTANCE.askListEmployee = true;
            Communication.INSTANCE.askEmployeeList();
            this.imageCalendar.setEnabled(false);
            this.imageEmployee.setEnabled(false);
            this.imageVideo.setEnabled(false);
            this.imagePorte.setEnabled(false);
        }
    }
    /**
     * @brief Navigue vers un autre fragment lorsque l'image Vidéo est appuyé.
     * @param v [in] Permet de déclencher l'événement de click sur l'image Vidéo.
     */
    public void onClickVideo(View v)
    {
        if (v.getId() == R.id.buttonVideo)
        {
            ConnectionManager.INSTANCE.cnt = CNT_VIDEO;
            Navigation.findNavController(v).navigate(R.id.fragmentVideo);
        }
    }
    /**
     * @brief Navigue vers un autre fragment lorsque l'image Contrôle porte est appuyé.
     * @param v [in] Permet de déclencher l'événement de click sur l'image Contrôle porte.
     */
    public void onClickDoor(View v)
    {
        if (v.getId() == R.id.buttonDoor)
        {
            ConnectionManager.INSTANCE.cnt = CNT_DOOR;
            Navigation.findNavController(v).navigate(R.id.fragmentDoor);
        }
    }
    /**
     * @brief Navigue vers un autre fragment lorsque l'image Employés est appuyé.
     * @param v [in] Permet de déclencher l'événement de click sur l'image Employés.
     */
    public void onClickEmployee(View v)
    {
        if (v.getId() == R.id.buttonEmployee)
        {
            Communication.INSTANCE.askListEmployee = false;
            Communication.INSTANCE.askEmployeeList();
            this.imageCalendar.setEnabled(false);
            this.imageEmployee.setEnabled(false);
            this.imageVideo.setEnabled(false);
            this.imagePorte.setEnabled(false);
        }
    }
}
