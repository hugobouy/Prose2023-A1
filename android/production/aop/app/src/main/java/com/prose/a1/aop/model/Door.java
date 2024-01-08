/**
 * @file 	Door.java
 * @brief	Classe permettant de contrôler la porte
 * @author 	CASSAR Bastien & Mathis Moulin
 * @date 	08/06/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */
package com.prose.a1.aop.model;

import androidx.lifecycle.MutableLiveData;


public class Door
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief LiveData pour la porte
     */
    private final MutableLiveData<Integer> stateDoor = new MutableLiveData<>();
    /**
     * @brief singleton.
     */
    public static Door INSTANCE = new Door();

    /*
     * FUNCTIONS
     */
    /**
     * @brief Obtient l'état de la porte
     * @return stateDoor.
     */
    public MutableLiveData<Integer> getStateDoor()
    {
        return stateDoor;
    }
    /**
     * @brief Change l'état de la porte
     */
    public void setDoorState(byte stateDoor)
    {
        if (convertBinaireToString(stateDoor).equals("1")) getStateDoor().postValue(1);
        else getStateDoor().postValue(0);
    }
    /**
     * @brief converti le binaire en String
     * @param messageEncode [in] binaire à convertir
     */
    private String convertBinaireToString(byte messageEncode)
    {
        return String.format("%c", messageEncode);
    }
}
