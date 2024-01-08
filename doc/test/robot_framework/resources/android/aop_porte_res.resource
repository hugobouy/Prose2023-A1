#
# Ressource RobotFramework pour le test de l'affichage du calendrier.
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
Library    OperatingSystem
Library    Pillow



*** Variables ***

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity
${AOP_APK}            ${EXECDIR}/SUT/aop.apk

${SCREENSHOT_PATH}    /report/porte/screenshot.png


${STATUS_INDICATOR}    id=com.prose.a1.aop:id/statusIndicator
${GREEN_COLOR}          #2ECC71
${RED_COLOR}            #E74C3C

*** Keywords ***

Démonstrateur demande à contrôler la Porte
    Wait Until Element Is Visible    id=textHome
    Click Element    id=buttonDoor

AOP affiche Écran_Porte
    Wait Until Page Contains Element    id=${AOP_PACKAGE}:id/btnUnlock

Démonstrateur demande à ouvrir la Porte
    Log    Test couleur bouton
    #${couleur_indicateur} =  Get Element Attribute    ${STATUS_INDICATOR}    background-color
    #${is_red}    Compare Colors    ${couleur_indicateur}    255    0    0    50    # Vérifie si la couleur est proche du vert (ajustez les valeurs selon vos besoins)
    #Log    résulat : ${is_red}
    Vérifier Couleur Indicateur de Statut
    Click Element    id=btnUnlock
#    Log    Ouvrture de la Porte

AOP change l'état de la Porte à ouvert pendant TOP
    #${couleur_indicateur} =  Get Element Attribute    ${STATUS_INDICATOR}     background-color
    #${is_green}    Compare Colors    ${couleur_indicateur}    0    255    0    50    # Vérifie si la couleur est proche du vert (ajustez les valeurs selon vos besoins)
    #Log    résulat : ${is_green}
    Vérifier Couleur Indicateur de Statut

AOP change l'état de la Porte à fermer
    #${couleur_indicateur} =  Get Element Attribute    ${STATUS_INDICATOR}     background-color
    #${is_red}    Compare Colors    ${couleur_indicateur}    255    0    0    50    # Vérifie si la couleur est proche du vert (ajustez les valeurs selon vos besoins)
    #Log    résulat : ${is_red}
    Vérifier Couleur Indicateur de Statut

Démonstrateur demande à quitter le contrôle de la Porte
    Click Element    id=buttonReturnDoor

Analyze Color
    [Arguments]    ${image_path}
    ${image}    Open Image    ${image_path}
    ${pixels}    Get Data    ${image}
    ${color}    Get Pixel Color    ${pixels}    0    0    # Obtient la couleur du premier pixel de l'image
    [Return]    ${color}

Vérifier Couleur Indicateur de Statut
    Capture Element Screenshot    ${ELEMENT_ID}    ${SCREENSHOT_PATH}
    ${color}    Run Keyword And Return Status    Analyze Color    ${SCREENSHOT_PATH}
    Log    La couleur de l'élément est : ${color
