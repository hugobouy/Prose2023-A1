#
# Ressource RobotFramework pour l'écran de connexion.
#
# Version pour l'implémentation Android de AOP.
#
#
# @version 0.1
# @author Bastien Cassar
#

*** Settings ***

Library    Process
Library    AppiumLibrary
Library    String
Library    DateTime

*** Variables ***
${HOME_TEXT}    id=textHome
${WAIT_TIME}    5   # Temps d'attente en secondes

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity

*** Keywords ***

L'utilisateur lance l'exécution du composant AOP
    Log    yo
    switch application    Aop
    activate application    ${AOP_PACKAGE}

Démonstrateur entre les informations_de_connexion
    [Arguments]    ${pass}
    input text    id=passwordConnect    ${pass}

Démonstrateur se connecte
    Click Element    id=buttonConnexion

AOP affiche PopUp_Attente_Connexion
    Wait Until Page Contains    Connexion en cours ...

SoftSonnette informe AOP de la connexion
    Wait Until Element Is Visible    id=textHome

AOP affiche PopUp_Erreur_MDP_Admin
    Wait Until Page Contains    Mauvais mot de passe

AOP affiche PopUp_Erreur_Connexion
    Wait Until Page Contains    Erreur de connexion

Démonstrateur valide le message
    #J'ai réussi à trouver le boutton grâce à la commande adb shell uiautomator dump qui a inspecté les éléments de l'interface du téléphone.
    #Puis adb pull /sdcard/window_dump.xml afin d'avoir un fichier xml avec toutes les infos.
    Click Element    id=android:id/button1

AOP affiche l'écran de connexion
    Wait Until Element Is Visible    id=text_connexion

AOP se ferme
    Click Element    id=buttonQuit
    sleep    1s
