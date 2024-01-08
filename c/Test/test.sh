#!/bin/bash

#Chemin vers les programmes à éxécuter

testGuard="./softSonnette/testGuard/testGuard"  
testUISS="./softSonnette/testUISS/testUISS"

#Exécution des programmes

echo "Test de Guard"
$testGuard

echo "Test de UISS"
$testUISS

echo "Fin des tests avec Cmocka."
