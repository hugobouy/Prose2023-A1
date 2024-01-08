/**
 * @file 	FragmentListEmployee.java
 * @brief	Classe permettant d'afficher l'écran de la liste des employés
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
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.prose.a1.aop.R;
import com.prose.a1.aop.communication.Communication;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;
import com.prose.a1.aop.model.employeeManager.EmployeeAdapter;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;

public class FragmentListEmployee extends Fragment
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief création de la variable builder pour la Pop-Up Erreur de Connexion.
     */
    AlertDialog.Builder builder;
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief instance du viewModel de connexion
     */
    ViewModelConnection viewModelConnection;
    View view;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    public FragmentListEmployee() {
        // Required empty public constructor
        Log.d(TAG, "$$$ Fragment ListEmploye $$$");
    }
    /**
     * @brief Permet de créer la vue du fragment avec les éléments du .xml.
     * @param inflater [in] créer une instance de view à partir du fichier xml du calendrier.
     * @param container [in] Conteneur parent dans lequel la vue sera créer.
     * @param savedInstanceState [in] contient les données de l'état précédent la vue.
     * @return Retourne la vue créée
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_list_employee, container, false);
        (view.findViewById(R.id.button)).setOnClickListener(this::onClickReturn);
        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(requireActivity()).get(ViewModelConnection.class);
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
        {
            if (value == 5) errorConnectionEmployeeListPopUp(view);
        });
        // Récupérer le bouton "button_add_employee" depuis la vue
        Button buttonAddEmployee = view.findViewById(R.id.button_add_employee);

        // Gérer le clic sur le bouton "button_add_employee"
        buttonAddEmployee.setOnClickListener(v -> {
            // Naviguer vers le fragmentAddEmployee
            ConnectionManager.INSTANCE.cnt = 8;
            Navigation.findNavController(view).navigate(R.id.fragmentAddEmployee);
        });

        // Récupérer le RecyclerView depuis la vue
        RecyclerView recyclerView = view.findViewById(R.id.recycler_view);

        // Créer et définir l'adaptateur
        EmployeeAdapter employeeAdapter = new EmployeeAdapter(getContext());

        //Définir le LayoutManager et l'adaptateur pour le RecyclerView :
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        recyclerView.setAdapter(employeeAdapter);

        return view;
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionEmployeeListPopUp(View displayConnection)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = 1;
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
     * @brief Navigue vers un autre fragment lorsque le bouton return est appuyé.
     */
    public void onClickReturn(View view)
    {
        if (view.getId() == R.id.button)
        {
            Navigation.findNavController(view).navigate(R.id.fragmentHome);
            Log.i(TAG, "Click return");
            ConnectionManager.INSTANCE.cnt = 2;
        }
    }
}
