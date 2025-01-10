#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Fonction pour initialiser le disque (mémoire secondaire)
bool initialiserDisque(SystemeFichiers* fs, int nbBlocs, int tailleBloc) {
    if (nbBlocs > MAX_BLOCKS) {
        printf("Erreur: Nombre de blocs dépasse la limite maximale.\n");
        return false;
    }

    fs->nbBlocsTotaux = nbBlocs;
    fs->tailleBloc = tailleBloc;
    fs->nbFichiers = 0;

    // Initialiser tous les blocs comme libres
    for (int i = 0; i < MAX_BLOCKS; i++) {
        fs->tableAllocationBlocs[i] = false;
        fs->memoireSecondaire[i].estOccupe = false;
        fs->memoireSecondaire[i].blocSuivant = -1;
        memset(fs->memoireSecondaire[i].nomFichier, 0, MAX_FILENAME_LENGTH);
        for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            fs->memoireSecondaire[i].enregistrements[j].id = -1;
            fs->memoireSecondaire[i].enregistrements[j].estSupp = false;
            memset(fs->memoireSecondaire[i].enregistrements[j].info, 0, sizeof(fs->memoireSecondaire[i].enregistrements[j].info));
        }
    }

    printf("Disque initialisé avec %d blocs de taille %d.\n", nbBlocs, tailleBloc);
    return true;
}

// Fonction pour lire un bloc du disque
void lireBloc(SystemeFichiers* fs, int numeroBloc, Bloc* bloc) {
    if (bloc == NULL || numeroBloc < 0 || numeroBloc >= fs->nbBlocsTotaux) {
        printf("Erreur: Numéro de bloc invalide.\n");
        return;
    }

    *bloc = fs->memoireSecondaire[numeroBloc];
}

// Fonction pour écrire un bloc sur le disque
void ecrireBloc(SystemeFichiers* fs, int numeroBloc, Bloc* bloc) {
    if (bloc == NULL || numeroBloc < 0 || numeroBloc >= fs->nbBlocsTotaux) {
        printf("Erreur: Numéro de bloc invalide.\n");
        return;
    }

    fs->memoireSecondaire[numeroBloc] = *bloc;
}

// Fonction pour allouer des blocs contigus
int allouerBlocsContigus(SystemeFichiers* fs, int nbBlocsDemandes) {
    int blocsConsecutifs = 0;
    int premierBloc = -1;

    for (int i = 0; i < fs->nbBlocsTotaux; i++) {
        if (!fs->tableAllocationBlocs[i]) {
            if (blocsConsecutifs == 0) premierBloc = i;
            blocsConsecutifs++;

            if (blocsConsecutifs == nbBlocsDemandes) {
                for (int j = premierBloc; j < premierBloc + nbBlocsDemandes; j++) {
                    fs->tableAllocationBlocs[j] = true;
                    fs->memoireSecondaire[j].estOccupe = true;
                    fs->memoireSecondaire[j].blocSuivant = (j < premierBloc + nbBlocsDemandes - 1) ? j + 1 : -1;
                }
                return premierBloc;
            }
        } else {
            blocsConsecutifs = 0;
            premierBloc = -1;
        }
    }

    printf("Erreur: Pas assez de blocs contigus disponibles.\n");
    return -1;
}

// Fonction pour allouer des blocs chaînés
int* allouerBlocsChaines(SystemeFichiers* fs, int nbBlocsDemandes) {
    int* blocsAlloues = (int*)malloc(nbBlocsDemandes * sizeof(int));
    if (blocsAlloues == NULL) {
        printf("Erreur: Impossible d'allouer de la mémoire.\n");
        return NULL;
    }

    int nbTrouves = 0;
    for (int i = 0; i < fs->nbBlocsTotaux && nbTrouves < nbBlocsDemandes; i++) {
        if (!fs->tableAllocationBlocs[i]) {
            blocsAlloues[nbTrouves++] = i;
            fs->tableAllocationBlocs[i] = true;
            fs->memoireSecondaire[i].estOccupe = true;
        }
    }

    if (nbTrouves == nbBlocsDemandes) {
        for (int i = 0; i < nbTrouves; i++) {
            fs->memoireSecondaire[blocsAlloues[i]].blocSuivant = (i < nbTrouves - 1) ? blocsAlloues[i + 1] : -1;
        }
        return blocsAlloues;
    } else {
        for (int i = 0; i < nbTrouves; i++) {
            fs->tableAllocationBlocs[blocsAlloues[i]] = false;
            fs->memoireSecondaire[blocsAlloues[i]].estOccupe = false;
        }
        free(blocsAlloues);
        printf("Erreur: Pas assez de blocs disponibles.\n");
        return NULL;
    }
}

// Fonction pour libérer des blocs
void libererBlocs(SystemeFichiers* fs, int* numerosBlocs, int nbBlocs) {
    for (int i = 0; i < nbBlocs; i++) {
        if (numerosBlocs[i] >= 0 && numerosBlocs[i] < fs->nbBlocsTotaux) {
            fs->tableAllocationBlocs[numerosBlocs[i]] = false;
            fs->memoireSecondaire[numerosBlocs[i]].estOccupe = false;
            fs->memoireSecondaire[numerosBlocs[i]].blocSuivant = -1;
            memset(fs->memoireSecondaire[numerosBlocs[i]].nomFichier, 0, MAX_FILENAME_LENGTH);
            for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
                fs->memoireSecondaire[numerosBlocs[i]].enregistrements[j].id = -1;
                fs->memoireSecondaire[numerosBlocs[i]].enregistrements[j].estSupp = false;
                memset(fs->memoireSecondaire[numerosBlocs[i]].enregistrements[j].info, 0, sizeof(fs->memoireSecondaire[numerosBlocs[i]].enregistrements[j].info));
            }
        }
    }
}

// Fonction pour compacter le disque
void compacter(SystemeFichiers* fs) {
    int prochainBlocLibre = 0;

    for (int i = 0; i < fs->nbFichiers; i++) {
        if (!fs->metadonneesFichiers[i].estActif) continue;

        int ancienPremierBloc = fs->metadonneesFichiers[i].premierBloc;
        int blocCourant = ancienPremierBloc;
        int blocsTraites = 0;

        while (blocCourant != -1 && blocsTraites < fs->metadonneesFichiers[i].nbBlocs) {
            if (blocCourant != prochainBlocLibre) {
                // Copier le bloc vers le nouvel emplacement
                fs->memoireSecondaire[prochainBlocLibre] = fs->memoireSecondaire[blocCourant];
                fs->tableAllocationBlocs[prochainBlocLibre] = true;
                fs->tableAllocationBlocs[blocCourant] = false;
            }

            // Mettre à jour les pointeurs suivants
            if (blocsTraites < fs->metadonneesFichiers[i].nbBlocs - 1) {
                fs->memoireSecondaire[prochainBlocLibre].blocSuivant = prochainBlocLibre + 1;
            } else {
                fs->memoireSecondaire[prochainBlocLibre].blocSuivant = -1;
            }

            blocCourant = fs->memoireSecondaire[blocCourant].blocSuivant;
            prochainBlocLibre++;
            blocsTraites++;
        }

        // Mettre à jour les métadonnées du fichier
        fs->metadonneesFichiers[i].premierBloc = prochainBlocLibre - blocsTraites;
    }

    // Effacer les blocs restants
    for (int i = prochainBlocLibre; i < fs->nbBlocsTotaux; i++) {
        fs->tableAllocationBlocs[i] = false;
        fs->memoireSecondaire[i].estOccupe = false;
        fs->memoireSecondaire[i].blocSuivant = -1;
        memset(fs->memoireSecondaire[i].nomFichier, 0, MAX_FILENAME_LENGTH);
        for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            fs->memoireSecondaire[i].enregistrements[j].id = -1;
            fs->memoireSecondaire[i].enregistrements[j].estSupp = false;
            memset(fs->memoireSecondaire[i].enregistrements[j].info, 0, sizeof(fs->memoireSecondaire[i].enregistrements[j].info));
        }
    }

    printf("Compactage terminé : %d blocs compactés.\n", prochainBlocLibre);
}

// Fonction pour afficher l'état de la mémoire secondaire
void afficherEtatMemoireSecondaire(SystemeFichiers* fs) {
    int blocsLibres = 0;

    printf("\nEtat de la mémoire secondaire :\n");
    printf("Nombre total de blocs : %d\n", fs->nbBlocsTotaux);
    printf("Taille des blocs : %d octets\n", fs->tailleBloc);

    printf("\nDétails des blocs :\n");
    for (int i = 0; i < fs->nbBlocsTotaux; i++) {
        printf("Bloc %d : %s, Suivant: %d, Fichier: '%s'\n",
               i,
               fs->memoireSecondaire[i].estOccupe ? "Occupé" : "Libre",
               fs->memoireSecondaire[i].blocSuivant,
               fs->memoireSecondaire[i].nomFichier);

        if (!fs->memoireSecondaire[i].estOccupe) {
            blocsLibres++;
        }
    }

    printf("\nBlocs libres : %d\n", blocsLibres);
    printf("Blocs occupés : %d\n", fs->nbBlocsTotaux - blocsLibres);
}

// Fonction pour vider la mémoire secondaire
void viderMemoireSecondaire(SystemeFichiers* fs) {
    // Libérer tous les blocs
    for (int i = 0; i < fs->nbBlocsTotaux; i++) {
        fs->tableAllocationBlocs[i] = false;
        fs->memoireSecondaire[i].estOccupe = false;
        fs->memoireSecondaire[i].blocSuivant = -1;
        memset(fs->memoireSecondaire[i].nomFichier, 0, MAX_FILENAME_LENGTH);
        for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            fs->memoireSecondaire[i].enregistrements[j].id = -1;
            fs->memoireSecondaire[i].enregistrements[j].estSupp = false;
            memset(fs->memoireSecondaire[i].enregistrements[j].info, 0, sizeof(fs->memoireSecondaire[i].enregistrements[j].info));
        }
    }

    // Effacer toutes les métadonnées des fichiers
    for (int i = 0; i < fs->nbFichiers; i++) {
        fs->metadonneesFichiers[i].estActif = false;
        memset(fs->metadonneesFichiers[i].nomFichier, 0, MAX_FILENAME_LENGTH);
        fs->metadonneesFichiers[i].nbEnregistrements = 0;
        fs->metadonneesFichiers[i].nbBlocs = 0;
        fs->metadonneesFichiers[i].premierBloc = -1;
        fs->metadonneesFichiers[i].organisationGlobale = 0;
        fs->metadonneesFichiers[i].organisationInterne = 0;
    }

    // Réinitialiser le nombre de fichiers
    fs->nbFichiers = 0;

    printf("Mémoire secondaire vidée avec succès.\n");
}