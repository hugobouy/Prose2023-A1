#
# Ressource RobotFramework pour le test de la liste des employés et l'ajout et la suppression.
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


*** Variables ***

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity
${AOP_APK}            ${EXECDIR}/SUT/aop.apk


*** Keywords ***

Démonstrateur demande à voir la liste des employés
    Click Element    id=buttonEmployee

AOP affiche Écran_Liste
    Wait Until Element Is Visible    id=textView2

Démonstrateur demande à ajouter un employé
    Click Element    id=button_add_employee

AOP affiche Écran_Ajout
    Wait Until Element Is Visible    id=textView4

Démonstrateur valide l’ajout
    Click Element    id=ajouter_button

Démonstrateur demande à retourner à Écran_Accueil
    Click Element    id=button
    Wait Until Element Is Visible    id=textHome

Démonstrateur annule l’ajout
    Click Element    id=annuler_button

Démonstrateur demande à supprimer un employé
    Click Element    id=item_delete_button

AOP affiche PopUp_Suppression
    Wait Until Page Contains     Supprimer un employé

Démonstrateur valide la suppression
    Click Element    id=android:id/button1

Démonstrateur renseigne les informations de l’employé spécial
    [Arguments]    ${name}    ${first_name}
    input text    id=nom_edittext    ${name}
    input text    id=prenom_edittext    ${first_name}
    Click Element    id=button_import_photo
    Wait Until Page Contains    Récents
    Click text    ProSE-A1_Soutenance_Incrément2.png
    Wait Until Page Contains    Ajouter un nouvel employé
    Click Element    id=role_spinner
    Wait Until Page Contains    Employé Sécurité
    Click Element    xpath=//android.widget.ListView/android.widget.CheckedTextView[5]


    Click Element    id=lundi_debut_spinner
    # Scroll jusqu'à ce que le texte spécifique soit visible
    # Pour faire le scrolle j'ai utilisé uiautomator pour avoir les coordonées du spinner
    # Pour scroller utilisé une boucle for et swiper jusqu'à voir le texte souhaité.
    # Ensuite on swipe uniqueent vers le bas donc les coordonnées y seront modifiés. On trouve un coef grâce à uiautomator.
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '08h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   320   1398   320   1065   500
    END
    Click Text    08h 00
    Click Element    id=lundi_fin_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '18h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   550   1398   550   1065   500
    END
    Click Text    18h 00


    Click Element    id=mardi_debut_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '10h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   320   1398   320   1061   500
    END
    Click Text    10h 00
    Click Element    id=mardi_fin_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '15h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   550   1398   550   1061   500
    END
    Click Text    15h 00


    Click Element    id=mercredi_debut_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '09h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   320   1398   320   1081   500
    END
    Click Text    09h 00
    Click Element    id=mercredi_fin_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '19h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   550   1398   550   1081   450
    END
    Click Text    19h 00


    Click Element    id=jeudi_debut_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '07h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   320   1398   320   1101   500
    END
    Click Text    07h 00
    Click Element    id=jeudi_fin_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '20h 00')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   550   1398   550   1101   350
    END
    Click Text    20h 00

    Click Element    id=vendredi_debut_spinner
    Wait Until Page Contains    01h 00
    Click Element    xpath=//android.widget.ListView/android.widget.TextView[1]
    Click Element    id=vendredi_fin_spinner
    Wait Until Page Contains    01h 00
    Click Element    xpath=//android.widget.ListView/android.widget.TextView[1]

  Click Element    id=samedi_debut_spinner
    Wait Until Page Contains    01h 00
    Click Element    xpath=//android.widget.ListView/android.widget.TextView[2]
    Click Element    id=samedi_fin_spinner
    FOR    ${index}    IN RANGE    15
        ${is_visible}=    Run Keyword And Return Status    Page Should Contain Element    xpath=//*[contains(@text, '23h 30')]
        Exit For Loop If    '${is_visible}' == 'True'
        Swipe   550   1398   550   1045   100
    END
    Click Text    23h 30

    Click Element    id=dimanche_debut_spinner
    Wait Until Page Contains    01h 00
    Click Element    xpath=//android.widget.ListView/android.widget.TextView[1]
    Click Element    id=dimanche_fin_spinner
    Wait Until Page Contains    01h 00
    Click Element    xpath=//android.widget.ListView/android.widget.TextView[1]


Démonstrateur renseigne les informations de l’employé
    [Arguments]    ${name}    ${first_name}
    input text    id=nom_edittext    ${name}
    input text    id=prenom_edittext    ${first_name}
    Click Element    id=button_import_photo
    Wait Until Page Contains    Récents
    Click text    ProSE-A1_Soutenance_Incrément2.png
    Wait Until Page Contains    Ajouter un nouvel employé
    Click Element    id=role_spinner
    Wait Until Page Contains    Employé Sécurité
    Click Element    xpath=//android.widget.ListView/android.widget.CheckedTextView[4]