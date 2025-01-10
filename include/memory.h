#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

TableAllocation lireTableAllocation() ;
void sauvegarderTableAllocation(TableAllocation *table);
// Initialisation du disque
bool initialiserDisque(SystemeFichiers* fs, int nbBlocs, int tailleBloc);

// Fonctions de gestion des blocs
void lireBloc(SystemeFichiers* fs, int numeroBloc, Bloc* bloc);
void ecrireBloc(SystemeFichiers* fs, int numeroBloc, Bloc* bloc);

// Allocation de blocs
int allouerBlocsContigus(SystemeFichiers* fs, int nbBlocsDemandes);
int* allouerBlocsChaines(SystemeFichiers* fs, int nbBlocsDemandes);

// Libération des blocs
void libererBlocs(SystemeFichiers* fs, int* numerosBlocs, int nbBlocs);

// Gestion de la mémoire secondaire
void compacter(SystemeFichiers* fs);
void viderMemoireSecondaire(SystemeFichiers* fs);

// Affichage de l'état de la mémoire secondaire
void afficherEtatMemoireSecondaire(SystemeFichiers* fs);


#endif

