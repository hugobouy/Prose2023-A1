#
# Ressource RobotFramework pour le test de l'affichage du calendrier.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#

*** Settings ***

Resource    resources/android/aop_calendrier_res.resource

*** Variables ***

*** Keywords ***

# Précondition : Se connecter + Avoir les employés requis dans la liste

Consulter calendrier
    [Arguments]     ${employee}    ${index}
    # 1.
    Démonstrateur demande à consulter le calendrier
    # 2. #3. interne
    # 4.
    AOP affiche Écran_Calendrier
    # 5.
    Démonstrateur demande à voir le calendrier d'un employé    ${employee}  ${index}
    # 6.
    AOP affiche Écran_Calendrier avec la variante liée à l'employé    ${employee}
    # 7.
    Démonstrateur demande à quitter le calendrier
