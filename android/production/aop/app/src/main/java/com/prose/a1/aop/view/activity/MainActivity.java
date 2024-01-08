/**
 * @file 	MainActivity.java
 * @brief	Classe Activity de aop.
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
package com.prose.a1.aop.view.activity;

import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;
import android.os.Bundle;
import android.util.Log;

import com.prose.a1.aop.R;
import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;

public class MainActivity extends AppCompatActivity
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief instance de la navigation
     */
    NavController navController;

    /*
     * FUNCTIONS
     */
    /**
     * @brief création de l'activité avec la mise en place d'une navigation entre les fragments.
     * @param savedInstanceState If the activity is being re-initialized after
     *     previously being shut down then this Bundle contains the data it most
     *     recently supplied in {@link #onSaveInstanceState}.  <b><i>Note: Otherwise it is null.</i></b>
     *
     */
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        NavHostFragment navHostFragment = (NavHostFragment) getSupportFragmentManager().findFragmentById(R.id.fragNavig);
        assert navHostFragment != null;
        navController = navHostFragment.getNavController();
    }
    /**
     * @brief Désactive le bouton return "natif" à Android
     */
    @Override
    public void onBackPressed()
    {
        // Ne rien faire pour supprimer le comportement du bouton de retour natif
        if (CacheEmployeeManager.INSTANCE.callback)
        {
            Log.e("TAG", "mainActivity");
            navController.navigate(R.id.fragmentAddEmployee);
        }
    }
}
