#
# Ressource RobotFramework pour le test de la navigation dans AOP.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#


*** Settings ***

Resource    aop_accueil.resource
#Resource    aop_calendrier.resource
Resource    aop_connexion.resource
Resource    resources/android/launch_aop.resource

Suite Setup         Exécution SUT (sur cible)
Suite Teardown      Arrêt SUT (sur cible)

*** Test Cases ***

Naviguer dans AOP
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Naviguer dans AOP
#    Stopper la connexion
#    Arrêt softSonnette