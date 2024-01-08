#
# Ressource RobotFramework pour l'écran de connexion.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Bastien Cassar
#


*** Settings ***
Suite Setup         Exécution SUT (sur cible)
Suite Teardown      Arrêt SUT (sur cible)
Resource    resources/android/launch_aop.resource
Resource    aop_connexion.resource



*** Variables ***
${PASS_SOFTSONNETTE}                1234
${PASS_DEFAUT}                      ${PASS_SOFTSONNETTE}

*** Test Cases ***

Etablir la connexion
    [Teardown]    Arrêt AOP
    Exécution softSonnette
    Etablir la connexion
    Stopper la connexion
    Arrêt softSonnette

Etablir la connexion [Mauvais mot de passe]
    [Teardown]    Arrêt AOP
    Exécution softSonnette
    Etablir la connexion [Mauvais mot de passe]
    Arrêt softSonnette

Etablir la connexion [Connexion non établie après TAC]
    [Teardown]    Arrêt AOP
    Etablir la connexion [Connexion non établie après TAC]



