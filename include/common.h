#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BLOCKS 1000
#define BLOCK_SIZE 10
#define MAX_FILES 100
#define MAX_RECORDS_PER_BLOCK 10
#define MAX_FILENAME_LENGTH 100


typedef struct {
    int id;
    char info[100];
    bool estSupp;
} Enregistrement;

typedef struct {
    char nomFichier[MAX_FILENAME_LENGTH];
    int nbEnregistrements;
    int premierBloc;
    int nbBlocs;
    int organisationGlobale; // 0: Contigu, 1: Chaîné
    int organisationInterne; // 0: Trié, 1: Non trié
    bool estActif;
} Metadonnees;

typedef struct {
    char nomFichier[MAX_FILENAME_LENGTH];
    Enregistrement enregistrements[MAX_RECORDS_PER_BLOCK];
    int blocSuivant;
    bool estOccupe;
} Bloc;

typedef struct {
    int taillebloc;
    int nbblocs;
    char bitmap[1000];
} TableAllocation;

typedef struct {
    Bloc memoireSecondaire[MAX_BLOCKS];
    Metadonnees metadonneesFichiers[MAX_FILES];
    int nbFichiers;
    int nbBlocsTotaux;
    int tailleBloc;
    bool tableAllocationBlocs[MAX_BLOCKS];
} SystemeFichiers;

#endif // COMMON_H