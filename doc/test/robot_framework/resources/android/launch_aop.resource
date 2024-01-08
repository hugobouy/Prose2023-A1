#
# Ressource RobotFramework pour afin de lancer les tests.
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

# Localisation de appium (ex: /home/<login>/Appium/node_modules/.bin/appium)
${APPIUM}              /home/forter/Appuim/node_modules/.bin/appium

# Localisation du SDK Android (ex: /home/<login>/Android/Sdk/)
${ANDROID_HOME}         /home/forter/Documents/S8/android/sdk/

${AOP_PACKAGE}        com.prose.a1.aop
${AOP_ACTIVITY}       view.activity.MainActivity
${AOP_APK}            ${EXECDIR}/SUT/aop.apk

${SOFT_SONNETTE}    ${EXECDIR}/SUT/serveur/

*** Keywords ***


Exécution SUT (sur cible)
    Exécution Appium
    Déployer AOP (sur cible)


Arrêt SUT (sur cible)
    Fermer AOP (sur cible)
    Arrêt Appium


Exécution Appium
    start process       ${APPIUM}
    ...                 -p    4723    -a    127.0.0.1    -pa    /wd/hub    --allow-insecure\=adb_shell
    ...                 env:ANDROID_HOME=${ANDROID_HOME}
    ...                 alias=Appium
    sleep    3s

Arrêt Appium
    terminate process    Appium

Déployer AOP (sur cible)
    open application    http://127.0.0.1:4723/wd/hub    allowTestPackages=true    automationName=UIAutomator2    platformName=Android
    ...                 app=${AOP_APK}    appPackage=${AOP_PACKAGE}    appActivity=${AOP_ACTIVITY}
    ...                 alias=Aop

Fermer AOP (sur cible)
    switch application    Aop
    close application

Arrêt AOP
    switch application    Aop
    stop application    ${AOP_PACKAGE}

Exécution softSonnette
    start process    ${SOFT_SONNETTE}/softSonnette.out  alias=Serveur
    sleep    1s

Arrêt softSonnette
    terminate process    Serveur

