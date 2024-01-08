#
# Ressource RobotFramework pour les tests liés à la liste des employés.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Bastien Cassar
#


*** Settings ***

Resource    aop_liste.resource
Resource    aop_connexion.resource
Resource    resources/android/launch_aop.resource

Suite Setup         Exécution SUT (sur cible)
Suite Teardown      Arrêt SUT (sur cible)

*** Test Cases ***

Consulter la liste des employés
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Consulter la liste des employés
#    Stopper la connexion
#    Arrêt softSonnette

Annuler ajout d'un employé
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Annuler ajout d'un employé liste remplie
    Annuler ajout d'un employé liste vide
#    Stopper la connexion
#    Arrêt softSonnette

Ajouter un employé [employé non spécial]
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Ajouter un employé [employé non spécial]
#    Stopper la connexion
#    Arrêt softSonnette

Supprimer un employé
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Supprimer un employé
#    Stopper la connexion
#    Arrêt softSonnette

Ajouter un employé [employé spécial]
    [Teardown]    Arrêt AOP
#    Exécution softSonnette
    Etablir la connexion
    Ajouter un employé [employé spécial]
#    Stopper la connexion
#    Arrêt softSonnette

