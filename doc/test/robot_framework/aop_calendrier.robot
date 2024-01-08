#
# Ressource RobotFramework pour le test de l'affichage du calendrier.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#


*** Settings ***

Resource    aop_calendrier.resource
Resource    aop_connexion.resource
Resource    resources/android/launch_aop.resource

Suite Setup         Exécution SUT (sur cible)
Suite Teardown      Arrêt SUT (sur cible)

*** Test Cases ***

Consulter calendrier
    [Teardown]    Arrêt AOP
    Run Keywords
#        Exécution softSonnette
        Etablir la connexion
        Consulter calendrier    Chiron    1
        Consulter calendrier    Cassar    2
#        Stopper la connexion
#        Arrêt softSonnette
#        Consulter calendrier    employee_sécurité
#        Consulter calendrier    employee_soir
#        Consulter calendrier    employee_spécial
