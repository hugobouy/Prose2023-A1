/**
 * @file 	EmployeeAdapter.java
 * @brief	Classe permettant de faire la communication
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

package com.prose.a1.aop.model.employeeManager;

import android.app.AlertDialog;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.prose.a1.aop.R;
import com.prose.a1.aop.communication.Communication;

import java.util.List;

public class EmployeeAdapter extends RecyclerView.Adapter<EmployeeAdapter.EmployeeViewHolder>
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief Tag pour utiles aux déboguages.
     */
    private static final String TAG = "TAG";
    /**
     * @brief Variable définissant la nouvelle liste utile à l'affichage de la liste via le RecylcerView.
     */
    public String[][] listEmployeeRecyclerView;

    /**
     * @brief Variable définissant le context de la class.
     */
    private final Context context;

    /**
     * @brief Variable définissant le temps du dernier clique, afin d'éviter l'effet de rebond.
     */
    private long lastClickTime = 0;

    /**
     * @brief Variable définissant la liste des IDs des employés.
     */
    public List<Integer> employeeIDs;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Constructeur de la classe.
     * @param context Context de la classe.
     */
    public EmployeeAdapter(Context context)
    {
        this.context = context;
        List<String> employeeNames = CacheEmployeeManager.INSTANCE.getNameListEmployee();
        List<String> employeeFirstNames = CacheEmployeeManager.INSTANCE.getFirstNameListEmployee();
        List<Integer> employeeRoles = CacheEmployeeManager.INSTANCE.getRoleListEmployee();
        employeeIDs = CacheEmployeeManager.INSTANCE.getIdEmployee();

        int nbEmployee = employeeNames.size();
        listEmployeeRecyclerView = new String[nbEmployee][3];
        for(int i=0; i < nbEmployee; i++){
            listEmployeeRecyclerView[i][0] = employeeFirstNames.get(i);
            listEmployeeRecyclerView[i][1] = employeeNames.get(i);
            listEmployeeRecyclerView[i][2] = getRole(employeeRoles.get(i));
        }
    }
    /**
     * @brief Fonction qui obtient le rôle correspondant à l'identifiant donné.
     * @param id L'identifiant du rôle.
     * @return Le nom du rôle correspondant à l'identifiant donné.
     */
    private String getRole(int id)
    {
        String[] roleNames = context.getResources().getStringArray(R.array.roles);
        String role = roleNames[0];
        Log.e(TAG, "Role et id : " + role);
        if(id < roleNames.length){
            role = roleNames[id];
        }
        return role;
    }
    /**
     * @brief Fonction qui crée un nouveau ViewHolder en inflatant la vue de l'élément de la liste.
     * @param parent Le ViewGroup dans lequel la nouvelle vue sera ajoutée après avoir été liée à une position d'adaptateur.
     * @param viewType Le type de vue de la nouvelle vue.
     * @return Un nouveau ViewHolder contenant la vue de l'élément de la liste.
     */
    @NonNull
    @Override
    public EmployeeViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType)
    {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_employee, parent, false);
        return new EmployeeViewHolder(itemView);
    }
    /**
     * @brief Méthode qui met à jour le ViewHolder pour représenter le contenu de l'élément à la position donnée dans l'ensemble de données de l'adaptateur.
     * @param holder Le ViewHolder qui doit être mis à jour pour représenter le contenu de l'élément.
     * @param position La position de l'élément dans l'ensemble de données de l'adaptateur.
     */
    @Override
    public void onBindViewHolder(@NonNull EmployeeViewHolder holder, int position)
    {
        String[] employeeData = listEmployeeRecyclerView[position];
        holder.nomTextView.setText(employeeData[1]);
        holder.prenomTextView.setText(employeeData[0]);
        holder.roleTextView.setText(employeeData[2]);
        holder.deleteButton.setOnClickListener(v -> removeItem(holder.itemView.getContext(), position));
    }
    /**
     * @brief Fonction qui renvoie le nombre total d'éléments dans l'ensemble de données de l'adaptateur.
     * @return Le nombre total d'éléments dans l'ensemble de données.
     */
    @Override
    public int getItemCount()
    {
        return listEmployeeRecyclerView.length;
    }
    /**
     * @brief Méthode qui supprime l'employé à la position donnée de la liste des employés.
     * @param context Le contexte de l'application.
     * @param position La position de l'élément à supprimer.
     */
    public void removeItem(Context context, int position)
    {
        long currentTime = System.currentTimeMillis();
        if (currentTime - lastClickTime < 650) { // Délai de 0,65 seconde entre les clics pour éviter le rebond
            return;
        }
        lastClickTime = currentTime;
        // Vérification de la position valide
        if (position < 0 || position >= listEmployeeRecyclerView.length)
        {
            return;
        }
        //Affichage et gestion de la Pop-pup
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(R.string.popUp_title_delete);
        builder.setMessage(context.getString(R.string.popup_mess_delete) + listEmployeeRecyclerView[position][0]
                + " " + listEmployeeRecyclerView[position][1] + " ?");
        builder.setPositiveButton(R.string.popup_delete_p_b, (dialog, which) ->
        {
            // Suppression de l'élément à la position donnée en cas d'appuie sur 'Continuer'
            String[][] newList = new String[listEmployeeRecyclerView.length - 1][3];
            System.arraycopy(listEmployeeRecyclerView, 0, newList, 0, position);
            System.arraycopy(listEmployeeRecyclerView, position + 1, newList, position, listEmployeeRecyclerView.length - position - 1);
            listEmployeeRecyclerView = newList;

            // Mise à jour de l'adaptateur
            notifyItemRemoved(position);
            notifyItemRangeChanged(position, listEmployeeRecyclerView.length);

            // Calcul de l'id de l'employé à supprimer
            int idSupressEmployee = employeeIDs.get(position);

            // Envoie de l'iD de l'employé à supprimer
            employeeIDs.remove(position);
            Log.d(TAG, "Employé avec l'id : "+ idSupressEmployee + "  à supprimer"+employeeIDs);
            Communication.INSTANCE.deleteEmployee(idSupressEmployee);

        });

        builder.setNegativeButton(R.string.popup_delete_n_b, (dialog, which) -> dialog.dismiss());
        AlertDialog dialog = builder.create();
        dialog.show();
    }
    /**
     * @brief Class qui représente un ViewHolder pour afficher les éléments de la liste des employés.
     */
    public static class EmployeeViewHolder extends RecyclerView.ViewHolder {

        public TextView nomTextView;
        public TextView prenomTextView;
        public TextView roleTextView;
        public Button deleteButton;

        public EmployeeViewHolder(@NonNull View itemEmployeeView) {
            super(itemEmployeeView);
            nomTextView = itemEmployeeView.findViewById(R.id.item_lastname);
            prenomTextView = itemEmployeeView.findViewById(R.id.item_firstname);
            roleTextView = itemEmployeeView.findViewById(R.id.item_role);
            deleteButton = itemEmployeeView.findViewById(R.id.item_delete_button);
        }
    }
}