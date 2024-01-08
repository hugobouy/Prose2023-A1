/**
 * @file 	example.java
 * @brief	Sert de modèle pour le Projet Prose A1
 * @author 	TROVALLET Romain
 * @date 	10/03/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */



/**
 * @brief An example class
 */
public class ClassExample
{	

	/*
	 * LOCAL VARIABLES
	 */
		
	
	/*
	 * @brief Exemple d'énumération
	 */
	public enum Level {
		LOW,
		MEDIUM,
		HIGH,
		ULTRA
	}
	
	
	/*
	 * @brief Attribut de la classe se basant sur une énumération
	 */
	public Level myLevel;
	
	
	/**
	 * @brief Exemple de valeur qui n'apparaît pas dans le doxygen (sauf ajout d'un attribut public devant)
	 */
	private static int exampleValue;
	
	/**
	 * @brief Exemple d'une constante 
	 */
	public static final double EPSILON = 0.2;

	/**
	 * @brief Exemple d'un tableau qui n'apparaît pas dans le doxygen (sauf ajout d'un attribut public devant)
	 */
	private int exampleTable [];
	
	
	/*
	 * FUNCTIONS
	 */
		
	/**
	 * @brief Constructeur de la classe par défaut
	 */	
	public ClassExample ()
	{
		this.myLevel = MEDIUM;
		this.exampleValue = 0;
		this.exampleTable = new int [20];	
	}
	
	/**
	 * @brief Constructeur de la classe avec initialisation de la valeur
	 * @param [in] valueInit Valeur pour remplir la classe
	 * @overload ClassExample ()
	 */	
	public ClassExample (int valueInit)
	{
		this.myLevel = MEDIUM;
		this.exampleValue = valueInit;
		this.exampleTable = new int [20];	
	}
	
	
	/**
	 * @brief Constructeur de la classe avec initialisation de l'énumération
	 * @param [in] levelInit Enumération pour remplir la classe
	 * @overload ClassExample ()
	 */	
	public ClassExample (Level levelInit)
	{
		this.myLevel = levelInit;
		this.exampleValue = 0;
		this.exampleTable = new int [20];	
	}
	
	
	
	/**
	 * @brief Getteur de la classe
	 * @return exampleValue: Retourne la valeur de la classe
	 */
	public int getValue()
	{
		return exampleValue;
	}
	
	/**
	 * @brief Setteur de la classe
	 * @param [in] value Remplie la valeur de la classe
	 */
	public void setValue(int value)
	{
		exampleValue = value;
	}
	
	/**
	 * @brief Incrémentation de la valeur de ClasseExample
	 * @param [in] increment Valeur de  la valeur de la classe
	 */
	public void incrementValue(int increment)
	{
		exampleValue+= increment;
	}
	
	/**
	 * @brief Recherche d'une valeure dans le tableau de ClasseExample
	 * @param [in] value Valeure cherchée dans le tableau
	 * @return returnMessage: Message retournant les détails de la recherche
	 */
	public String searchTable(int value)
	{
		int i;
		boolean exit = FALSE;
		String returnMessage;
		
		for (i=0; i<20 && !exit; i++)
		{
			if(exampleTable[i] == value)
			{
				exit = TRUE;
			}
			else
			{
				i++;
			}
		}
		if (exit)
		{
			returnMessage = "Value " + value +
					 " has been found at " + i + 
					 "in the table";
		}
		else
		{
			returnMessage = "The value has not been found";
		}
			
		return returnMessage;
	}
}





