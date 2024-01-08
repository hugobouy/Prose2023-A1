/**
 * @file 	FragmentConnection.java
 * @brief	Classe du fragment de connexion
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

package com.prose.a1.aop.view.fragment;

import android.app.AlertDialog;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;

import android.os.Handler;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;

import com.prose.a1.aop.R;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;

public class FragmentConnection extends Fragment implements View.OnClickListener {

    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur échec de mot de passe
     */
    private static final int BAD_PASS = 0;
    /**
     * @brief valeur validation de mot de passe
     */
    private static final int GOOD_PASS = 1;
    /**
     * @brief valeur erreur de connexion dans l'accueil
     */
    private static final int ERREUR_CONNECTION = 1;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran connexion
     */
    private static final int CNT_CONNECTION = 1;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran home
     */
    private static final int CNT_HOME = 2;
    /**
     * @brief création de la variable pour la popUp d'attente.
     */
    AlertDialog popUpWaitingErrorConnect;
    /**
     * @brief création de la variable pour la popUp d'attente.
     */
    AlertDialog popUpWaitingConnect;
    /**
     * @brief création de la variable pour la popUp d'attente.
     */
    AlertDialog popUpWaitingWrongPass;
    /**
     * @brief mot de passe entré par l'utilisateur dans l'application
     */
    EditText editPassword;
    /**
     * @brief adresse Ip entré par l'utilisateur dans l'application
     */
    EditText editIp;
    /**
     * @brief instance du viewModel de connexion
     */
    ViewModelConnection viewModelConnection;
    /**
     * @brief variable de la vue du fragment
     */
    View root;
    /**
     * @brief permet de créer les popUp.
     */
    AlertDialog.Builder builder;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur
     */
    public FragmentConnection()
    {
        // Required empty public constructor
    }

    /**
     * @brief crée la vue du fragment.
     * @param inflater The LayoutInflater object that can be used to inflate
     * any views in the fragment,
     * @param container If non-null, this is the parent view that the fragment's
     * UI should be attached to.  The fragment should not add the view itself,
     * but this can be used to generate the LayoutParams of the view.
     * @param savedInstanceState If non-null, this fragment is being re-constructed
     * from a previous saved state as given here.
     *
     * @return la vue
     */
    @Override
    public View onCreateView( LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        // Inflate the layout for this fragment
        this.root = inflater.inflate(R.layout.fragment_connection, container, false);
        this.root.findViewById(R.id.buttonConnexion).setOnClickListener(this);
        editPassword = this.root.findViewById(R.id.passwordConnect);
        editIp = this.root.findViewById(R.id.enterIP);
        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(this).get(ViewModelConnection.class);
        viewModelConnection.getWrongFormatIp().observe(getViewLifecycleOwner(), value -> wrongFormatIp());
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == ERREUR_CONNECTION)
                    {
                        waitingConnectionPopUpForErrorConnect();
                    }
                });
        viewModelConnection.getValidPass().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == GOOD_PASS)
                    {
                        ConnectionManager.INSTANCE.getLocalTime();
                        waitingConnectionPopUpForHome();
                    }
                    else if (value == BAD_PASS) waitingConnectionPopUpForWrongPass();
                });
        return root;
    }
    /**
     * @brief Affiche un toast signalant que l'adresse Ip ne correspond pas au format.
     */
    private void wrongFormatIp()
    {
        Runnable runnable = () ->
                Toast.makeText(getActivity(), R.string.ToastWrongIP, Toast.LENGTH_LONG).show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Affiche un toast signalant qu'il manque des informations.
     */
    private void missInformations()
    {
        Runnable runnable = () ->
            Toast.makeText(getActivity(), R.string.ToastMissingInfo, Toast.LENGTH_LONG).show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Crée un popUp d'attente pour l'échec de la connexion.
     */
    private void waitingConnectionPopUpForErrorConnect()
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(requireActivity());
        builder.setMessage(R.string.PopUpWaitingConnection);
        builder.setCancelable(false);

        this.popUpWaitingErrorConnect = builder.create();
        this.popUpWaitingErrorConnect.show();
        // Écouteur de fermeture de la première pop-up
        popUpWaitingErrorConnect.setOnDismissListener(dialog ->
        {
            // Afficher la deuxième pop-up après la fermeture de la première
            errorConnectionPopUp();
        });

        // Programmer la fermeture de la pop-up après 5 secondes
        new Handler().postDelayed(() ->
        {
            if (this.popUpWaitingErrorConnect != null && popUpWaitingErrorConnect.isShowing())
            {
                popUpWaitingErrorConnect.dismiss(); // Fermer la pop-up si elle est toujours affichée
            }
        }, 5000);
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionPopUp()
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_CONNECTION;
                ConnectionManager.INSTANCE.initErrorConnection();
                AlertDialog.Builder builder = new AlertDialog.Builder(requireActivity());
                builder.setMessage(R.string.PopUpErrorConnection);
                builder.setPositiveButton(R.string.BoutonReturn, (dialog, which) ->
                {
                    // Action à effectuer lorsque le bouton OK est cliqué
                    dialog.dismiss(); // Fermer la pop-up
                });
                builder.setCancelable(false); // Empêcher la fermeture de la pop-up lors du clic en dehors d'elle
                AlertDialog dialog = builder.create();
                dialog.show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Crée un popUp d'attente de connexion pour le mot de passe.
     */
    private void waitingConnectionPopUpForWrongPass()
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(requireActivity());
        builder.setMessage(R.string.PopUpWaitingConnection);
        builder.setCancelable(false);

        this.popUpWaitingWrongPass = builder.create();
        this.popUpWaitingWrongPass.show();
        // Écouteur de fermeture de la première pop-up
        popUpWaitingWrongPass.setOnDismissListener(dialog -> {
            // Afficher la deuxième pop-up après la fermeture de la première
            wrongPasswordPopUp();
        });

        // Programmer la fermeture de la pop-up après 5 secondes
        new Handler().postDelayed(() ->
        {
            if (this.popUpWaitingWrongPass != null && popUpWaitingWrongPass.isShowing())
            {
                popUpWaitingWrongPass.dismiss(); // Fermer la pop-up si elle est toujours affichée
            }
        }, 2000);
    }
    /**
     * @brief affiche un popUp signalant que le mot de passe est incorrect.
     */
    private void wrongPasswordPopUp()
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_CONNECTION;
                AlertDialog.Builder builder = new AlertDialog.Builder(requireActivity());
                builder.setMessage(R.string.PopUpWrongPass);
                builder.setPositiveButton(R.string.BoutonReturn, (dialog, which) ->
                {
                    // Action à effectuer lorsque le bouton OK est cliqué
                    dialog.dismiss(); // Fermer la pop-up
                });
                builder.setCancelable(false); // Empêcher la fermeture de la pop-up lors du clic en dehors d'elle
                AlertDialog dialog = builder.create();
                dialog.show();
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Crée un popUp pour l'entré sur l'écran home.
     */
    private void waitingConnectionPopUpForHome()
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(requireActivity());
        builder.setMessage(R.string.PopUpWaitingConnection);
        builder.setCancelable(false);

        this.popUpWaitingConnect = builder.create();
        this.popUpWaitingConnect.show();
        // Écouteur de fermeture de la première pop-up
        popUpWaitingConnect.setOnDismissListener(dialog -> {
            // Afficher la deuxième pop-up après la fermeture de la première
            changeFrag();
        });

        // Programmer la fermeture de la pop-up après 5 secondes
        new Handler().postDelayed(() ->
        {
            if (this.popUpWaitingConnect != null && popUpWaitingConnect.isShowing())
            {
                popUpWaitingConnect.dismiss(); // Fermer la pop-up si elle est toujours affichée
            }
        }, 2000);
    }
    /**
     * @brief change de fragment suite à un mot de passe correct.
     */
    private void changeFrag()
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_HOME;
                ConnectionManager.INSTANCE.initErrorConnection();
                Navigation.findNavController(this.root).navigate(R.id.fragmentHome);
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief demande la connexion au serveur suite au click sur le bouton.
     */
    public void onClick(View view)
    {
        if (view.getId() == R.id.buttonConnexion)
        {
            if ((!TextUtils.isEmpty(editIp.getText().toString())) && (!TextUtils.isEmpty(editPassword.getText().toString())))
            {
                ConnectionManager.INSTANCE.askConnection(editIp.getText().toString(), editPassword.getText().toString());
            }
            else missInformations();
        }
    }
}
