#
# Ressource RobotFramework pour le test de l'ouverture de la Porte.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Romain Trovallet
#


*** Settings ***

Resource    aop_porte.resource
#Resource    aop_calendrier.resource
Resource    aop_connexion.resource
Resource    resources/android/launch_aop.resource

Suite Setup         Exécution SUT (sur cible)
Suite Teardown      Arrêt SUT (sur cible)

*** Test Cases ***

Contrôler Porte à distance
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    # Wait    5s
    Contrôler Porte à distance
#    Stopper la connexion
#    Arrêt softSonnette