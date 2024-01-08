/**
 * @file 	FragmentDoor.java
 * @brief	Classe du fragment de la controle de la porte.
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
import static com.prose.a1.aop.R.string.toast_open_door_mess;

import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

import com.prose.a1.aop.R;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;
import com.prose.a1.aop.communication.Communication;
import com.prose.a1.aop.view.viewModel.ViewModelDoor;


public class FragmentDoor extends Fragment
{

     /*
     * LOCAL VARIABLES
     */
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief permet de faire les popUp.
     */
    AlertDialog.Builder builder;
    /**
     * @brief instance de viewModelConnection
     */
    ViewModelConnection viewModelConnection;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de la porte.
     */
    private static final int ERROR_CONNECTION = 4;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de connexion
     */
    private static final int CNT_CONNECTION = 1;
    /**
     * @brief Indicateur d'état de la porte.
     */
    private View statusIndicator;
    /**
     * @brief État de la porte.
     */
    private boolean doorState = false;
    /**
     * @brief instance du viewModel de la porte
     */
    ViewModelDoor viewModelDoor;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    public FragmentDoor()
    {
        // Required empty public constructor
        Log.d(TAG, "$$$ Fragment Door $$$");
    }
    /**
     * @brief Fonction qui crée et retourne la vue associée au fragment.
     * @param inflater L'objet LayoutInflater qui peut être utilisé pour gonfler toutes les vues dans le fragment.
     * @param container Si non null, c'est la vue parent à laquelle l'interface utilisateur du fragment doit être attachée. Le fragment ne doit
     *                  pas ajouter la vue lui-même, mais ceci peut être utilisé pour générer les LayoutParams de la vue.
     * @param savedInstanceState Si non null, ce fragment est reconstruit à partir d'un état précédemment enregistré tel que donné ici.
     * @return La vue associée au fragment.
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_door, container, false);
        this.builder = new AlertDialog.Builder(requireActivity());
        Communication.INSTANCE.askDoorState();
        viewModelConnection = new ViewModelProvider(requireActivity()).get(ViewModelConnection.class);
        viewModelDoor = new ViewModelProvider(this).get(ViewModelDoor.class);

        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == ERROR_CONNECTION) errorConnectionDoorPopUp(view);
                });
        viewModelDoor.getStateDoor().observe(getViewLifecycleOwner(), value ->
                {
                    Log.e(TAG, "getStateDoor : " + viewModelDoor.getStateDoor().getValue());
                    if (value == 1) changeStateDoor(true);
                    else if(value == 0) changeStateDoor(false);
                });

        // Référence à la vue statusIndicator
        statusIndicator = view.findViewById(R.id.statusIndicator);

        // Référence au bouton unlockButton
        Button unlockButton = view.findViewById(R.id.btnUnlock);

        // Demande d'ouverture porte
        unlockButton.setOnClickListener(v ->
        {
            if (!doorState)
            {
                // Envoie de la demande d'ouverture
                Communication.INSTANCE.askOpenDoor();
            }
            else
            {
                Toast.makeText(getActivity(), toast_open_door_mess, Toast.LENGTH_SHORT).show();
            }
        });
        // Récupérer le bouton "bouttonReturn" depuis la vue
        Button bouttonReturn = view.findViewById(R.id.buttonReturnDoor);

        // Gérer le clic sur le bouton "bouttonReturn"
        bouttonReturn.setOnClickListener(v -> {
            // Naviguer vers le fragmentHome
            ConnectionManager.INSTANCE.cnt = 2;
            Navigation.findNavController(view).navigate(R.id.fragmentHome);
        });

        return view;
    }
    /**
     * @brief Change l'état de la porte
     */
    private void changeStateDoor(boolean stateDoor)
    {
        if (stateDoor)
        {
            doorState = true;
            // Changer la couleur en vert
            statusIndicator.setBackgroundResource(R.color.green);
        }
        else
        {
            doorState = false;
            // Changer la couleur en rouge
            statusIndicator.setBackgroundResource(R.color.red);
        }
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionDoorPopUp(View displayConnection)
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
}
