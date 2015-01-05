#! /usr/bin/octave -qf
clear
printf("Debut la generation des différents codebook\n");
# Chargement de la librairie necessaire
pkg load signal
train('dataBase/Gareth/',4,1);
train('dataBase/Damien/',4,2);
train('dataBase/Alexandre/',4,3);
train('dataBase/Samuel/',4,4);
printf("Fin\n"); 
