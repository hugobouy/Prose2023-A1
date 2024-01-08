#
# Ressource RobotFramework pour le test de la navigation dans AOP.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#

*** Settings ***

Resource    resources/android/aop_accueil_res.resource


*** Variables ***

*** Keywords ***

# Précondition : Se connecter

Naviguer dans AOP
    #1.
    Démonstrateur navigue dans la liste des employés
    #2.
    Démonstrateur navigue dans le calendrier
    #3.
    Démonstrateur navigue dans l'affichage de la vidéo
    #4.
    Démonstrateur navigue dans le contrôle de la Porte