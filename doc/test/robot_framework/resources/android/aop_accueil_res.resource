#
# Ressource RobotFramework pour les tests liés à la liste des employés.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Bastien Cassar
#

*** Settings ***

Library    Process
Library    AppiumLibrary
Library    String
Library    DateTime
Library    BuiltIn
#Library    Pillow
Library    OperatingSystem

Resource    aop_liste_res.resource
Resource    aop_calendrier_res.resource
Resource    aop_porte_res.resource


*** Variables ***

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity
${AOP_APK}            ${EXECDIR}/SUT/aop.apk


*** Keywords ***

Démonstrateur navigue dans la liste des employés
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.
    Démonstrateur demande à retourner à Écran_Accueil


Démonstrateur navigue dans le calendrier
    #1.
    Démonstrateur demande à consulter le calendrier
    #2.
    AOP affiche Écran_Calendrier
    #3.
    Démonstrateur demande à quitter le calendrier

Démonstrateur navigue dans l'affichage de la vidéo
    #1.
    Démonstrateur demande à regarder la vidéo
    #2.
    AOP affiche Écran_Video
    #3.
    Démonstrateur demande à quitter la vidéo

Démonstrateur navigue dans le contrôle de la Porte
    #1.
    Démonstrateur demande à contrôler la Porte
    #2.
    AOP affiche Écran_Porte
    #3.
    Démonstrateur demande à quitter le contrôle de la Porte


Démonstrateur demande à regarder la vidéo
    Wait Until Element Is Visible    id=textHome
    Click Element    id=buttonVideo

AOP affiche Écran_Video
    Wait Until Element Is Visible    id=buttonReturnHome

Démonstrateur demande à quitter la vidéo
    Click Element    id=buttonReturnHome

