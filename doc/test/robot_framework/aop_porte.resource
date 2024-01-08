#
# Ressource RobotFramework pour le test de l'ouverture de la Porte.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#

*** Settings ***

Resource    resources/android/aop_porte_res.resource

*** Variables ***

*** Keywords ***

# Précondition : Se connecter

Contrôler Porte à distance
    # 1.
    Démonstrateur demande à contrôler la Porte
    # 2. interne
    # 3.
    AOP affiche Écran_Porte
    # 4.
    Démonstrateur demande à ouvrir la Porte
    # 5. interne
    # 6. => CU Ouvrir Porte 1.
    AOP change l'état de la Porte à ouvert pendant TOP
    # CU Ouvrir Porte 2. 3. intenes
    # CU Ouvrir Porte 4.
    AOP change l'état de la Porte à fermer
    # CU Ouvrir Porte 5. interne
    # 7.
    Démonstrateur demande à quitter le contrôle de la Porte