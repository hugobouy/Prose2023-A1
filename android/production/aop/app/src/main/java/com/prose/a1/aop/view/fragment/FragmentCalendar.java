/**
 * @file 	FragmentCalendar.java
 * @brief	Classe affichant le fragment du calendrier
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

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.prose.a1.aop.R;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;
import com.prose.a1.aop.model.employeeManager.WeeklyCalendar;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;

import java.util.List;


public class FragmentCalendar extends Fragment
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief valeur erreur de connexion dans le calendrier
     */
    private static final int ERREUR_CONNECTION = 3;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran home
     */
    private static final int CNT_HOME = 2;
    /**
     * @brief valeur de la constante lors d'une erreur de connexion pour l'écran de connexion
     */
    private static final int CNT_CONNECTION = 1;
    /**
     * @brief Variable faisant le lien entre le layout et le code
     */
    private View root;
    /**
     * @brief Variable faisant le lien avec la classe weeklyCalendar
     */
    WeeklyCalendar weeklyCalendar;
    /**
     * @brief Variable faisant le lien avec la classe Spinner
     */
    Spinner spinnerCalendar;
    /**
     * @brief permet de créer les popUp.
     */
    AlertDialog.Builder builder;
    /**
     * @brief instance de viewModelConnection
     */
    ViewModelConnection viewModelConnection;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Création du constructeur du fragment. Doit être vide.
     */
    public FragmentCalendar()
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
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // Inflate the layout for this fragment
        this.root = inflater.inflate(R.layout.fragment_calendar, container, false);
        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(requireActivity()).get(ViewModelConnection.class);
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == ERREUR_CONNECTION) errorConnectionCalendarPopUp(root);
                });
        root.findViewById(R.id.buttonReturnCalendar).setOnClickListener(this::onClickReturn);
        weeklyCalendar = root.findViewById(R.id.weeklyCalendar);
        setupSpinner();
        return this.root;
    }
    /**
     * @brief Affiche un popUp signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionCalendarPopUp(View displayConnection)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = CNT_CONNECTION;
                ConnectionManager.INSTANCE.initErrorConnection();
                CacheEmployeeManager.INSTANCE.deleteCache();
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
     * @param displayHome [in] Permet de déclencher l'événement de click sur le bouton return.
     * @brief Permet de revenir à l'écran d'accueil via le bouton return.
     */
    private void onClickReturn(View displayHome)
    {
        if (displayHome.getId() == R.id.buttonReturnCalendar)
        {
            ConnectionManager.INSTANCE.cnt = CNT_HOME;
            Navigation.findNavController(displayHome).navigate(R.id.fragmentHome);
        }
    }
    /**
     * @brief Création du spinner permettant de choisir un employé pour afficher son agenda.
     */
    private void setupSpinner()
    {
        spinnerCalendar = root.findViewById(R.id.employeeSpinner);
        List<String> employeeList = CacheEmployeeManager.INSTANCE.getNameListEmployee();
        ArrayAdapter<String> adapter = new ArrayAdapter<>(requireContext(), android.R.layout.simple_spinner_item, employeeList);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerCalendar.setAdapter(adapter);
        spinnerCalendar.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                String selectedEmployee = parent.getItemAtPosition(position).toString();
                // Met à jour le calendrier selon l'employé sélectionné
                weeklyCalendar.editCalendar(selectedEmployee);
                weeklyCalendar.invalidate();
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent)
            {
                // Ne rien faire
            }
        });
    }
}
