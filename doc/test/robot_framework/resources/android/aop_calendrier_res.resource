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
Library    Dialogs
Library    BuiltIn

*** Variables ***

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity
${AOP_APK}            ${EXECDIR}/SUT/aop.apk

${HOURS}   	 49
${MATRICE_ID}    id/weeklyCalendar

*** Keywords ***


AOP affiche la vue connexion
    wait activity    .MainActivity    3

L'utilisateur modifie l'adresse IP
    [Arguments]    ${ip}
    input text    ${AOP_PACKAGE}:id/ip    ${ip}

L'utilisateur modifie le numéro du port
    [Arguments]    ${port}
    input text    ${AOP_PACKAGE}:id/port    ${port}


L'utilisateur demande à se connecter
    Click Element    id=${AOP_PACKAGE}:id/connexion


AOP affiche un message de connexion
    [Arguments]    ${message_connexion}    ${ip}    ${port}
    ${message} =    Format String    ${message_connexion}    ip=${ip}    port=${port}
    AOP affiche un message    ${message}

AOP affiche un message de connexion impossible
    [Arguments]    ${message_connexion_impossible}    ${ip}    ${port}    ${delai}
    ${message} =    Format String    ${message_connexion_impossible}    ip=${ip}    port=${port}
    wait until keyword succeeds    3x    ${delai}    AOP affiche un message    ${message}

AOP affiche un message
    [Arguments]    ${message}
    element text should be    ${AOP_PACKAGE}:id/information    ${message}


AOP affiche un message d'accueil dans le chat
    [Arguments]    ${message_accueil}
    element should contain text    ${AOP_PACKAGE}:id/messages    ${message_accueil}


Démonstrateur demande à consulter le calendrier
    Wait Until Element Is Visible    id=textHome
    Click Element    id=buttonCalendar

AOP affiche Écran_Calendrier
    Wait Until Page Contains Element    id=${AOP_PACKAGE}:id/employeeSpinner

Démonstrateur demande à voir le calendrier d'un employé
    [Arguments]    ${employee}    ${index}
    Click Element    id=${AOP_PACKAGE}:id/employeeSpinner
    Wait Until Page Contains    ${employee}
    Click Element    xpath=//android.widget.ListView/android.widget.CheckedTextView[${index}]

AOP affiche Écran_Calendrier avec la variante liée à l'employé
    [Arguments]    ${employee}

    Run Keyword    Run System Command    pause    shell=True

#    ${mon_sta} =    Trouver Heure Debut Travail    LUN
#    ${mon_end}
#    ${tue_sta} =    Trouver Heure Debut Travail    MAR
#    ${tue_end}
#    ${wed_sta} =    Trouver Heure Debut Travail    MER
#    ${wed_end}
#    ${thu_sta} =    Trouver Heure Debut Travail    JEU
#    ${thu_end}
#    ${fri_sta} =    Trouver Heure Debut Travail    VEN
#    ${fri_end}
#    ${sat_sta} =    Trouver Heure Debut Travail    SAM
#    ${sat_end}
#    ${sun_sta} =    Trouver Heure Debut Travail    DIM
#    ${sun_end}


Trouver le début des horaires d'une journée
    [Arguments]    ${DAY}
    FOR    ${hour}    IN    ${HOURS}
	${result}=    TesterVraiFaux     ${hour}     ${DAY}
    	IF    ${result}
		    ${day_sta} =    ${hour}
		    [return]    ${day_sta}
    END

Trouver la fin des horaires d'une journée
    [Arguments]    ${DAY} ${day_sta}
    ${hour} =    ${day_sta}
    FOR    ${hour}    IN    ${HOURS}
	${result} =    TesterVraiFaux    ${hour} ${DAY}
    	IF    !${result}
		    ${day_end} =    ${hour}
		    [return]    ${day_end}
    END

TesterVraiFaux
    [Arguments]    ${hour} ${DAY}
    Run Keyword If    '${hour}' > '4'
	    [return]    TRUE
    ELSE
	    [return]    FALSE



Trouver Heure Debut Travail
    [Arguments]    ${jour}
    ${image_path}    Capture Screen    # Capture d'écran de la matrice
    ${matrice}    Open    ${image_path}    # Ouvrir l'image à l'aide de la bibliothèque Python Pillow
    ${matrice_id}    Get Element    xpath=//[@id="${MATRICE_ID}"]    # Récupérer l'élément de la matrice
    ${cases}    Get WebElements    ${matrice_id}    xpath=./div    # Récupérer toutes les cases de la matrice
    ${heure_debut}    Set Variable    None
    ${jour_trouve}    Set Variable    False

    FOR    ${case}    IN    @{cases}
        ${style}    Get Element Attribute    ${case}    style    # Récupérer le style de l'élément (qui contient les informations de couleur)
        ${color}    Get Color From Style    ${style}    background-color    # Extraire la couleur de fond de l'élément
        ${is_green}    Compare Colors    ${color}    0    255    0    50    # Vérifier si la couleur est proche du vert (ajuster les valeurs selon vos besoins)

        ${jour_case}    Get Text    ${case}    # Récupérer le texte associé à la case (jour de la semaine)
        Run Keyword If    '${jour_case}' == '${jour}'    # Vérifier si le jour correspond
            ${heure_debut}    Set Variable    ${case}
            ${jour_trouve}    Set Variable    True
            Exit For

        Run Keyword If    '${jour_trouve}' == 'True'    # Si le jour a déjà été trouvé et que l'on n'est plus dans la séquence de travail
            ${is_white}    Compare Colors    ${color}    255    255    255    50    # Vérifier si la couleur est proche du blanc (ajuster les valeurs selon vos besoins)
            IF    '${is_white}'    # Si la case est blanche, la séquence de travail est terminée
                ${heure_debut}    Set Variable    None
                Exit For
        END
    Log ${heure_debut}
    [Return]    ${heure_debut}


Démonstrateur demande à quitter le calendrier
    Click Element    id=buttonReturnCalendar








