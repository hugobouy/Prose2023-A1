/**
 * @file 	FragmentAddEmployee.java
 * @brief	Classe permettant d'afficher l'écran d'ajout d'un employé
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
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.Navigation;

import com.prose.a1.aop.R;
import com.prose.a1.aop.communication.Communication;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;
import com.prose.a1.aop.model.connectionManager.ConnectionManager;
import com.prose.a1.aop.view.viewModel.ViewModelConnection;
import com.prose.a1.aop.view.viewModel.ViewModelEmployeeManager;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Arrays;

public class FragmentAddEmployee  extends Fragment {

    /*
     * LOCAL VARIABLES
     */
    boolean entryImport = false;
    /**
     * @brief tag pour les debbug
     */
    private static final String TAG = "TAG";
    /**
     * @brief Photo importe en byte
     */
    byte[] bytePicture;
    /**
     * @brief vue utilisé pour le FragmentAddEmployee.
     */
    View view;
    /**
     * @brief création de la variable builder pour la Pop-Up Erreur de Connexion.
     */
    AlertDialog.Builder builder;
    /**
     * @brief instance de viewModelConnection.
     */
    ViewModelConnection viewModelConnection;
    /**
     * @brief instance de viewModelEmployeeManager.
     */
    ViewModelEmployeeManager viewModelEmployeeManager;
    /**
     * @brief création de la variable du nom saisie par l'utilisateur.
     */
    private EditText nomEditText;
    /**
     * @brief création de la variable du prénom saisie par l'utilisateur.
     */
    private EditText prenomEditText;
    /**
     * @brief création de la variable du rôle choisie par l'utilisateur.
     */
    private Spinner role;
    /**
     * @brief création de la variable du rôle choisie par l'utilisateur.
     */
    private String[] roleNames;
    /**
     * @brief Button ajouter un employé.
     */
    Button addButton;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     */
    public FragmentAddEmployee()
    {
        // Required empty public constructor
        Log.d(TAG, "$$$ Fragment AddEmployee $$$");
    }
    /**
     * @brief Permet de créer la vue du fragment avec les éléments du .xml.
     * @param inflater [in] créer une instance de view à partir du fichier xml du calendrier.
     * @param container [in] Conteneur parent dans lequel la vue sera créer.
     * @param savedInstanceState [in] contient les données de l'état précédent la vue.
     * @return Retourne la vue créée.
     */
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // Inflate the layout for this fragment
        view = inflater.inflate(R.layout.fragment_add_employee, container, false);
        CacheEmployeeManager.INSTANCE.callback = false;
        // Récupérer le bouton "buttonReturnListEmployee" depuis la vue
        Button buttonReturnHome = view.findViewById(R.id.annuler_button);
        this.builder = new AlertDialog.Builder(requireActivity());
        viewModelConnection = new ViewModelProvider(requireActivity()).get(ViewModelConnection.class);
        viewModelConnection.getErrorConnection().observe(getViewLifecycleOwner(), value ->
                {
                    if (value == 8) errorConnectionAddEmployeePopUp(view);
                });
        viewModelEmployeeManager = new ViewModelProvider(requireActivity()).get(ViewModelEmployeeManager.class);
        viewModelEmployeeManager.getScreenEmployeeList().observe(getViewLifecycleOwner(), value ->
                {
                    if (value==1)askScreenEmployeeList(view);
                });
        // Gérer le clic sur le bouton "annuler"
        buttonReturnHome.setOnClickListener(v ->
        {
            // Naviguer vers le FragmentListEmployee
            CacheEmployeeManager.INSTANCE.callback = false;
            Navigation.findNavController(v).navigate(R.id.fragmentListEmployee);
        });

        nomEditText = view.findViewById(R.id.nom_edittext);
        prenomEditText = view.findViewById(R.id.prenom_edittext);

        role = view.findViewById(R.id.role_spinner);
        roleNames = getResources().getStringArray(R.array.roles);

        // Initialiser le spinner rôle avec "Employé matin" = position 0
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(requireContext(),
                R.array.roles, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        role.setAdapter(adapter);
        role.setSelection(0);


        Spinner[] daySpinners = new Spinner[]
                {
                view.findViewById(R.id.lundi_debut_spinner),
                view.findViewById(R.id.lundi_fin_spinner),
                view.findViewById(R.id.mardi_debut_spinner),
                view.findViewById(R.id.mardi_fin_spinner),
                view.findViewById(R.id.mercredi_debut_spinner),
                view.findViewById(R.id.mercredi_fin_spinner),
                view.findViewById(R.id.jeudi_debut_spinner),
                view.findViewById(R.id.jeudi_fin_spinner),
                view.findViewById(R.id.vendredi_debut_spinner),
                view.findViewById(R.id.vendredi_fin_spinner),
                view.findViewById(R.id.samedi_debut_spinner),
                view.findViewById(R.id.samedi_fin_spinner),
                view.findViewById(R.id.dimanche_debut_spinner),
                view.findViewById(R.id.dimanche_fin_spinner)
        };

        // Initialiser les spinners en fonction du rôle choisi
        role.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                String selectedRole = parent.getItemAtPosition(position).toString();

                // Initialiser les spinners en fonction du rôle choisi
                // Si c'est un Employé spécial :
                if (selectedRole.equals(roleNames[roleNames.length - 1]))
                {
                    initializeSpinners(selectedRole, daySpinners);
                    // Débloquer les spinners de jour
                    for (Spinner spinner : daySpinners)
                    {
                        spinner.setEnabled(true);
                    }
                }
                else
                {
                    // Bloquer les spinners de jour par défaut
                    for (Spinner spinner : daySpinners)
                    {
                        spinner.setEnabled(false);
                    }
                    initializeSpinners(selectedRole, daySpinners);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent)
            {

            }
        });

        this.addButton = view.findViewById(R.id.ajouter_button);
        Button importButton = view.findViewById(R.id.button_import_photo);
        addButton.setOnClickListener(v ->
                {
                    Log.e(TAG, "addButton");
                    addButton.setEnabled(false);
                    // Contrôle de la sélection d'une photo
                    addNewEmployee(daySpinners);

                });
        importButton.setOnClickListener(v ->
                {
                    entryImport = true;
                    CacheEmployeeManager.INSTANCE.callback = true;
                    Intent intent = new Intent();
                    intent.setType("image/*");
                    intent.setAction(Intent.ACTION_GET_CONTENT);
                    startActivityForResult(Intent.createChooser(intent, "Sélectionner une photo"), 1);
                });

        return view;
    }
    /**
     * @brief récupére la photo
     * @param requestCode The integer request code originally supplied to
     *                    startActivityForResult(), allowing you to identify who this
     *                    result came from.
     * @param resultCode The integer result code returned by the child activity
     *                   through its setResult().
     * @param data An Intent, which can return result data to the caller
     *               (various data can be attached to Intent "extras").
     */
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);
        try
        {
            if (data == null ||data.getData()==null)
            {
                Log.i(TAG, "Erreur dans l'importe getdata null");
                CacheEmployeeManager.INSTANCE.callback = true;
            }
            else
            {
                Bitmap bitmap = MediaStore.Images.Media.getBitmap(getContext().getContentResolver(), data.getData());
                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                bitmap.compress(Bitmap.CompressFormat.PNG, 1, stream);
                this.bytePicture = stream.toByteArray();
                CacheEmployeeManager.INSTANCE.callback = false;
            }
        }
        catch (IOException e)
        {
            Log.i(TAG, "Erreur dans l'importe" + e.getMessage());
            throw new RuntimeException(e);
        }
    }
    /**
     * @brief Affiche l'écran de la liste des employés.
     * @param displayScreenEmployeeList vue du fragment.
     */
    private void askScreenEmployeeList(View displayScreenEmployeeList)
    {
        Runnable runnable = () ->
                ConnectionManager.INSTANCE.cnt = 5;
                ConnectionManager.INSTANCE.initErrorConnection();
                CacheEmployeeManager.INSTANCE.initAskEmployee();
                Navigation.findNavController(displayScreenEmployeeList).navigate(R.id.fragmentListEmployee);
        requireActivity().runOnUiThread(runnable);
    }
    /**
     * @brief Affiche un Pop-Up signalant l'échec de la connexion avec le serveur.
     */
    private void errorConnectionAddEmployeePopUp(View displayConnection)
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
     * @brief Initialise tous les spinners en fonction du rôle que l'utilisateur a choisie.
     * @param selectedRole Le rôle sélectionné.
     * @param spinners Les spinners à initialiser.
     */
    private void initializeSpinners(@NonNull String selectedRole, Spinner[] spinners)
    {
        // Modifications des spinners par défaut en fonction du rôle choisi
        switch (selectedRole)
        {
            case "Employé Matin":
                for (int i = 0; i < 10; i += 2)
                {
                    spinners[i].setSelection(7); // Début à 03h
                    spinners[i+1].setSelection(27); // Fin à 13h
                }
                for (int i = 10; i < spinners.length; i++)
                {
                    spinners[i].setSelection(0); // RIEN
                }
                break;
            case "Employé Journée":
                for (int i = 0; i < 10; i += 2)
                {
                    spinners[i].setSelection(17); // Début à 08h
                    spinners[i+1].setSelection(41); // Fin à 20h
                }
                for (int i = 10; i < spinners.length; i++)
                {
                    spinners[i].setSelection(0); // RIEN
                }
                break;
            case "Employé Soir":
                for (int i = 0; i < 10; i += 2)
                {
                    spinners[i].setSelection(27); // Début à 13h
                    spinners[i+1].setSelection(47); // Fin à 23h
                }
                for (int i = 10; i < spinners.length; i++)
                {
                    spinners[i].setSelection(0); // RIEN
                }
                break;
            case "Employé Sécurité":
                for (int i = 0; i < spinners.length; i += 2)
                {
                    spinners[i].setSelection(1); // Début à 00h
                    spinners[i+1].setSelection(48); // Fin à 23h30
                }
                break;

            case "Employé Spécial":
                for (Spinner spinner : spinners)
                {
                    spinner.setSelection(0); // RIEN
                }
                break;
            default:
                break;
        }
    }
    /**
     * @brief Permet d'envoyer un nouvel employé à partir des données saisies par l'utilisateur.
     * @param daySpinners Les spinners représentant les horaires de travail.
     */
    private void addNewEmployee(Spinner[] daySpinners)
    {
        String newName = nomEditText.getText().toString();
        String newFirstName = prenomEditText.getText().toString();

        byte[] newWorkignsHours = new byte[14]; // initialisation d'un tableau vide

        String newRole = role.getSelectedItem() != null ? role.getSelectedItem().toString() : "Employé Matin";
        int rolePosition = role.getSelectedItemPosition();
        for (int i = 0; i < newWorkignsHours.length; i++)
        {
            int positionSpinner = daySpinners[i].getSelectedItemPosition();
            if(positionSpinner == 0)
            {
                newWorkignsHours[i] = (byte) 1;
            }
            else
            {
                newWorkignsHours[i] = (byte) (daySpinners[i].getSelectedItemPosition());
            }
        }

        // Toast si le nom ou le prénom est/sont null
        if (newName.trim().isEmpty() || newFirstName.trim().isEmpty())
        {
            Toast.makeText(getContext(), "Veuillez saisir un nom et un prénom", Toast.LENGTH_SHORT).show();
            addButton.setEnabled(true);
            return;
        }

        // Validation des longueurs des noms et prénoms
        if (newName.length() < 1 || newName.length() > 12) {
            Toast.makeText(getContext(), "Le nom doit avoir entre 1 et 12 caractères", Toast.LENGTH_SHORT).show();
            addButton.setEnabled(true);
            return;
        }
        if (newFirstName.length() < 2 || newFirstName.length() > 12) {
            Toast.makeText(getContext(), "Le prénom doit avoir entre 2 et 12 caractères", Toast.LENGTH_SHORT).show();
            addButton.setEnabled(true);
            return;
        }
        if (!entryImport)
        {
            Toast.makeText(getContext(), "Veuillez saisir sélectionner une image", Toast.LENGTH_SHORT).show();
            addButton.setEnabled(true);
            return;
        }

        // Toast si en cas d'employé spécial, vérification que les horaires saisies sont conformes
        if (newRole.equals("Employé Spécial"))
        {
            boolean isValid = true;
            for (int i = 0; i < daySpinners.length; i += 2)
            {
                Spinner debutSpinner = daySpinners[i];
                Spinner finSpinner = daySpinners[i + 1];

                // Vérifier que les spinners début et fin sont tous les deux remplis (évite qu'il n'y ait qu'un "RIEN")
                if (debutSpinner.getSelectedItemPosition() == 0 && finSpinner.getSelectedItemPosition() > 0 ||
                        debutSpinner.getSelectedItemPosition() > 0 && finSpinner.getSelectedItemPosition() == 0)
                {
                    isValid = false;
                    break;
                }
                // Vérifier que l'heure de début < à l'heure fin
                else if (debutSpinner.getSelectedItemPosition() > finSpinner.getSelectedItemPosition())
                {
                    isValid = false;
                    break;
                }
            }
            if (!isValid)
            {
                Toast.makeText(requireContext(), "Veuillez saisir des horaires cohérentes", Toast.LENGTH_SHORT).show();
                this.addButton.setEnabled(true);
                return;
            }
        }

        //Envoie des données du nouvel Employé au serveur
        Log.e("TAG", "addEmployee work: "+ Arrays.toString(newWorkignsHours));
        Log.e("TAG", "addEmployee name: "+ newName);
        Log.e("TAG", "addEmployee firstName: "+ newFirstName);
        Log.e("TAG", "addEmployee role: "+ rolePosition);
        entryImport = false;
        Communication.INSTANCE.addEmployee(newName, newFirstName, bytePicture,rolePosition, newWorkignsHours);
        Communication.INSTANCE.askListEmployee = false;
        CacheEmployeeManager.INSTANCE.deleteCache();
        try
        {
            Thread.sleep(500);
        }
        catch (InterruptedException e)
        {
            throw new RuntimeException(e);
        }
        Communication.INSTANCE.askEmployeeList();
    }
}
