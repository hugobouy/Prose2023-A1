#
# Ressource RobotFramework pour les tests liés à la liste des employés.
#
# Version pour l'implémentation Android de AOP.
#
# @version 0.1
# @author Bastien Cassar
#

*** Settings ***

Resource    resources/android/aop_liste_res.resource

*** Variables ***
${NAME}                Tiffany
${FIRST_NAME}          Povhete


*** Keywords ***

# Précondition : Se connecter

Consulter la liste des employés
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.
    Démonstrateur demande à retourner à Écran_Accueil


Annuler ajout d'un employé liste remplie
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.4. internes
    #5.
    Démonstrateur demande à ajouter un employé
    #6.
    AOP affiche Écran_Ajout
    #7.
    Démonstrateur renseigne les informations de l’employé    ${NAME}    ${FIRST_NAME}
    #8.
    Démonstrateur annule l’ajout
    #9. 10. 11. 12. internes
    #13.
    AOP affiche Écran_Liste
    #14.
    Démonstrateur demande à retourner à Écran_Accueil

Annuler ajout d'un employé liste vide
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.4. internes
    #5.
    Démonstrateur demande à ajouter un employé
    #6.
    AOP affiche Écran_Ajout
    #7.
    #Démonstrateur renseigne les informations de l’employé
    #8.
    Démonstrateur annule l’ajout
    #9. 10. 11. 12. internes
    #13.
    AOP affiche Écran_Liste
    #14.
    Démonstrateur demande à retourner à Écran_Accueil

Ajouter un employé [employé non spécial]
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.4. internes
    #5.
    Démonstrateur demande à ajouter un employé
    #6.
    AOP affiche Écran_Ajout
    #7.
    Démonstrateur renseigne les informations de l’employé    ${NAME}    ${FIRST_NAME}
    #8.
    Démonstrateur valide l’ajout
    #9. 10. 11. 12. internes
    #13.
    AOP affiche Écran_Liste
    #14.
    Démonstrateur demande à retourner à Écran_Accueil

Ajouter un employé [employé spécial]
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3. 4. internes
    #5.
    Démonstrateur demande à ajouter un employé
    #6.
    AOP affiche Écran_Ajout
    #7.
    Démonstrateur renseigne les informations de l’employé spécial    ${NAME}    ${FIRST_NAME}
    #8.
    Démonstrateur valide l’ajout
    #9. 10. 11. 12. internes
    #13.
    AOP affiche Écran_Liste
    #14.
    Démonstrateur demande à retourner à Écran_Accueil

Supprimer un employé
    #1.
    Démonstrateur demande à voir la liste des employés
    #2.
    AOP affiche Écran_Liste
    #3.4. internes
    #5.a.1.
    Démonstrateur demande à supprimer un employé
    #5.a.2.
    AOP affiche PopUp_Suppression
    #5.a.3.
    Démonstrateur valide la suppression
    #5.a.4. 5.a.5. internes
    #13.
    AOP affiche Écran_Liste
    #14.
    Démonstrateur demande à retourner à Écran_Accueil





