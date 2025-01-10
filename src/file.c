#include "../include/file.h"


// Créer un nouveau fichier
void creerFichier(SystemeFichiers* fs, char* nom, int nbEnregistrements, int organisationGlobale, int organisationInterne) {
    if (fs->nbFichiers >= MAX_FILES) {
        printf("Impossible , Limite maximale atteinte.\n");
        return;
    }

    // Vérifier si un fichier avec le même nom existe déjà
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, nom) == 0 && fs->metadonneesFichiers[i].estActif) {
            printf("Un fichier avec ce nom existe déjà\n");
            return;
        }
    }

    // Calculer le nombre de blocs nécessaires
    int blocsNecessaires = (nbEnregistrements + MAX_RECORDS_PER_BLOCK - 1) / MAX_RECORDS_PER_BLOCK;

    // Trouver des blocs libres en fonction de l'organisation globale
    int premierBloc = -1;
    if (organisationGlobale == 0) { // Contigu
        for (int i = 0; i <= fs->nbBlocsTotaux - blocsNecessaires; i++) {
            bool estLibre = true;
            for (int j = 0; j < blocsNecessaires; j++) {
                if (fs->tableAllocationBlocs[i + j]) {
                    estLibre = false;
                    break;
                }
            }
            if (estLibre) {
                premierBloc = i;
                break;
            }
        }
    } else { // Chaîné
        int count = 0;
        for (int i = 0; i < fs->nbBlocsTotaux && count < blocsNecessaires; i++) {
            if (!fs->tableAllocationBlocs[i]) {
                if (count == 0) premierBloc = i;
                count++;
            }
        }
    }

    if (premierBloc == -1) {
        printf("Pas assez de blocs libres pour créer le fichier.\n");
        return;
    }

    // Allouer les blocs
    int blocCourant = premierBloc;
    for (int i = 0; i < blocsNecessaires; i++) {
        fs->tableAllocationBlocs[blocCourant] = true;
        fs->memoireSecondaire[blocCourant].estOccupe = true;
        strcpy(fs->memoireSecondaire[blocCourant].nomFichier, nom);
        if (organisationGlobale == 1 && i < blocsNecessaires - 1) { // Chaîné
            fs->memoireSecondaire[blocCourant].blocSuivant = blocCourant + 1;
        }
        blocCourant++;
    }

    // Ajouter les métadonnées du fichier
    fs->metadonneesFichiers[fs->nbFichiers].nbEnregistrements = 0;
    fs->metadonneesFichiers[fs->nbFichiers].nbBlocs = blocsNecessaires;
    fs->metadonneesFichiers[fs->nbFichiers].premierBloc = premierBloc;
    fs->metadonneesFichiers[fs->nbFichiers].organisationGlobale = organisationGlobale;
    fs->metadonneesFichiers[fs->nbFichiers].organisationInterne = organisationInterne;
    fs->metadonneesFichiers[fs->nbFichiers].estActif = true;
    strcpy(fs->metadonneesFichiers[fs->nbFichiers].nomFichier, nom);

    fs->nbFichiers++;
    printf("Fichier créé avec succès.\n");
}

// Insérer un enregistrement dans un fichier
void insererEnregistrement(SystemeFichiers* fs, char* nomFichier, Enregistrement enregistrement) {
    int indexFichier = -1;
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, nomFichier) == 0 && fs->metadonneesFichiers[i].estActif) {
            indexFichier = i;
            break;
        }
    }

    if (indexFichier == -1) {
        printf("Fichier '%s' non trouvé.\n", nomFichier);
        return;
    }

    // Trouver le premier bloc du fichier
    int indexBloc = fs->metadonneesFichiers[indexFichier].premierBloc;

    // Insérer l'enregistrement dans le premier emplacement disponible
    while (indexBloc != -1) {
        for (int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) {
            if (fs->memoireSecondaire[indexBloc].enregistrements[i].id == -1 || fs->memoireSecondaire[indexBloc].enregistrements[i].estSupp) {
                fs->memoireSecondaire[indexBloc].enregistrements[i] = enregistrement;
                fs->metadonneesFichiers[indexFichier].nbEnregistrements++;
                printf("Enregistrement inséré dans le fichier '%s'.\n", nomFichier);
                return;
            }
        }
        indexBloc = fs->memoireSecondaire[indexBloc].blocSuivant;
    }

    printf("Pas d'espace disponible dans le fichier '%s'.\n", nomFichier);
}

// Rechercher un enregistrement par ID
int rechercherEnregistrement(SystemeFichiers* fs, char* nomFichier, int id) {
    int indexFichier = -1;
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, nomFichier) == 0 && fs->metadonneesFichiers[i].estActif) {
            indexFichier = i;
            break;
        }
    }

    if (indexFichier == -1) {
        printf("Fichier '%s' non trouvé.\n", nomFichier);
        return -1;
    }

    int indexBloc = fs->metadonneesFichiers[indexFichier].premierBloc;
    while (indexBloc != -1) {
        for (int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) {
            if (fs->memoireSecondaire[indexBloc].enregistrements[i].id == id && !fs->memoireSecondaire[indexBloc].enregistrements[i].estSupp) {
                printf("Enregistrement trouvé dans le fichier '%s' au bloc %d, enregistrement %d.\n", nomFichier, indexBloc, i);
                return indexBloc;
            }
        }
        indexBloc = fs->memoireSecondaire[indexBloc].blocSuivant;
    }

    printf("Enregistrement non trouvé dans le fichier \n");
    return -1;
}

// Supprimer un enregistrement (logique ou physique)
void supprimerEnregistrement(SystemeFichiers* fs, char* nomFichier, int id, bool estPhysique) {
    int indexFichier = -1;
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, nomFichier) == 0 && fs->metadonneesFichiers[i].estActif) {
            indexFichier = i;
            break;
        }
    }

    if (indexFichier == -1) {
        printf("Fichier '%s' non trouvé.\n", nomFichier);
        return;
    }

    int indexBloc = fs->metadonneesFichiers[indexFichier].premierBloc;
    while (indexBloc != -1) {
        for (int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) {
            if (fs->memoireSecondaire[indexBloc].enregistrements[i].id == id && !fs->memoireSecondaire[indexBloc].enregistrements[i].estSupp) {
                if (estPhysique) {
                    // Suppression physique de l'enregistrement
                    fs->memoireSecondaire[indexBloc].enregistrements[i].id = -1;
                    memset(fs->memoireSecondaire[indexBloc].enregistrements[i].info, 0, sizeof(fs->memoireSecondaire[indexBloc].enregistrements[i].info));
                } else {
                    // Suppression logique de l'enregistrement
                    fs->memoireSecondaire[indexBloc].enregistrements[i].estSupp = true;
                }
                fs->metadonneesFichiers[indexFichier].nbEnregistrements--;
                printf("Enregistrement supprimé du fichier \n");
                return;
            }
        }
        indexBloc = fs->memoireSecondaire[indexBloc].blocSuivant;
    }

    printf("Enregistrement non trouvé dans le fichier '%s'.\n", nomFichier);
}

// Renommer un fichier
void renommerFichier(SystemeFichiers* fs, char* ancienNom, char* nouveauNom) {
    int indexFichier = -1;
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, ancienNom) == 0 && fs->metadonneesFichiers[i].estActif) {
            indexFichier = i;
            break;
        }
    }

    if (indexFichier == -1) {
        printf("Fichier '%s' non trouvé.\n", ancienNom);
        return;
    }

    strcpy(fs->metadonneesFichiers[indexFichier].nomFichier, nouveauNom);
    printf("Fichier renommé de '%s' à '%s'.\n", ancienNom, nouveauNom);
}

// Supprimer un fichier
void supprimerFichier(SystemeFichiers* fs, char* nomFichier) {
    int indexFichier = -1;
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (strcmp(fs->metadonneesFichiers[i].nomFichier, nomFichier) == 0 && fs->metadonneesFichiers[i].estActif) {
            indexFichier = i;
            break;
        }
    }

    if (indexFichier == -1) {
        printf("Fichier '%s' non trouvé.\n", nomFichier);
        return;
    }

    // Libérer tous les blocs associés au fichier
    int indexBloc = fs->metadonneesFichiers[indexFichier].premierBloc;
    while (indexBloc != -1) {
        fs->tableAllocationBlocs[indexBloc] = false;
        fs->memoireSecondaire[indexBloc].estOccupe = false;
        memset(fs->memoireSecondaire[indexBloc].nomFichier, 0, MAX_FILENAME_LENGTH);
        indexBloc = fs->memoireSecondaire[indexBloc].blocSuivant;
    }

    // Marquer le fichier comme inactif
    fs->metadonneesFichiers[indexFichier].estActif = false;
    printf("Fichier '%s' supprimé.\n", nomFichier);
}

// Défragmenter le système de fichiers
void defragmenterSystemeFichiers(SystemeFichiers* fs) {
    if (fs->nbFichiers == 0) {
        printf("Aucun fichier à défragmenter.\n");
        return;
    }

    int prochainBlocLibre = 0;

    // Parcourir tous les fichiers actifs
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (!fs->metadonneesFichiers[i].estActif) continue;

        int ancienPremierBloc = fs->metadonneesFichiers[i].premierBloc;
        int blocCourant = ancienPremierBloc;
        int blocsTraites = 0;

        // Déplacer tous les blocs du fichier vers des emplacements contigus
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

    // Effacer les blocs restants (blocs libres)
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

    printf("Défragmentation terminée : %d blocs réorganisés.\n", prochainBlocLibre);
}

// Afficher les métadonnées des fichiers
void afficherMetadonnees(SystemeFichiers* fs) {
    printf("\nMétadonnées des fichiers :\n");
    for (int i = 0; i < fs->nbFichiers; i++) {
        if (fs->metadonneesFichiers[i].estActif) {
            printf("Fichier: %s, Enregistrements: %d, Blocs: %d, Premier Bloc: %d\n",
                   fs->metadonneesFichiers[i].nomFichier,
                   fs->metadonneesFichiers[i].nbEnregistrements,
                   fs->metadonneesFichiers[i].nbBlocs,
                   fs->metadonneesFichiers[i].premierBloc);
        }
    }
}