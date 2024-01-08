#
# Ressource RobotFramework pour l'écran de connexion.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Bastien Cassar
#

*** Settings ***

Resource    resources/android/aop_connexion.resource
Library    OperatingSystem
Library    BuiltIn

*** Variables ***
${PASS_SOFTSONNETTE}                1234
${PASS_DEFAUT}                      ${PASS_SOFTSONNETTE}
${WRONG_PASS}                       1879

*** Keywords ***


Etablir la connexion
    #0.
    L'utilisateur lance l'exécution du composant AOP
    #1.
    Démonstrateur entre les informations_de_connexion    ${PASS_DEFAUT}
    #2.
    Démonstrateur se connecte
    #3.
    AOP affiche PopUp_Attente_Connexion
    #4. #5. interne
    #6.
    SoftSonnette informe AOP de la connexion
    #7. interne

Stopper la connexion
    AOP se ferme

Etablir la connexion [Mauvais mot de passe]
    #0.
    L'utilisateur lance l'exécution du composant AOP
    #1.
    Démonstrateur entre les informations_de_connexion    ${WRONG_PASS}
    #2.
    Démonstrateur se connecte
    #3.
    AOP affiche PopUp_Attente_Connexion
    #4. #5. interne
    #5.a.1
    AOP affiche PopUp_Erreur_MDP_Admin
    #5.a.2.
    Démonstrateur valide le message

Etablir la connexion [Connexion non établie après TAC]
    #0.
    L'utilisateur lance l'exécution du composant AOP
    #1.
    Démonstrateur entre les informations_de_connexion    ${PASS_DEFAUT}
    #2.
    Démonstrateur se connecte
    #3.
    AOP affiche PopUp_Attente_Connexion
    #4. #5. interne
    #5-6.b.1
    AOP affiche PopUp_Erreur_Connexion
    #5-6.b.2.
    Démonstrateur valide le message
    #5-6.b.3
    AOP affiche l'écran de connexion