/**
 * @file 	guard.h
 * @brief 	Header du fichier guard.c
 * @author 	LETENNEUR Laurent
 * @date 	09/05/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

/*
 * INCLUDES
 */

#include "common.h"

#ifndef GUARD_H
#define GUARD_H

/**
 * @brief 
 * Fonction vérifiant la validité du mot de passe en comaparant deux valeurs
 * @post Lance la fonction validatePass() avec pour paramètre <b>1</b> si le mot de passe est correct ou <b>0</b> si le mot de passe est incorrect
 * 
 * @param [in] Password Mot de passe entré par le démonstrateur sur AOP
 */
void guard_askCheckPass (Password password);


#endif
