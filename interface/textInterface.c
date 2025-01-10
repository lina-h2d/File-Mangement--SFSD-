#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/file.h"
#include "../include/memory.h"

const char* filename = "memory.bin";

// Fonction pour sauvegarder automatiquement
void sauvegarderAutomatiquement(SystemeFichiers* fs) {
    FILE* fichier = fopen(filename, "wb");
    if (fichier == NULL) {
        printf("Erreur: Impossible de sauvegarder l'état du système de fichiers.\n");
        return;
    }

    fwrite(fs, sizeof(SystemeFichiers), 1, fichier);
    fclose(fichier);
    printf("État du système de fichiers sauvegardé dans '%s'.\n", filename);
}

// Fonction pour charger l'état au démarrage
bool chargerEtatSysteme(SystemeFichiers* fs) {
    FILE* fichier = fopen(filename, "rb");
    if (fichier == NULL) {
        printf("Aucun état précédent trouvé. Démarrage d'un nouveau système de fichiers.\n");
        return false;
    }

    fread(fs, sizeof(SystemeFichiers), 1, fichier);
    fclose(fichier);
    printf("État du système de fichiers chargé depuis '%s'.\n", filename);
    return true;
}

int afficherMenu() {
    SystemeFichiers fs;
    bool disqueInitialise = false;

    // Charger l'état du système de fichiers au démarrage
    if (chargerEtatSysteme(&fs)) {
        disqueInitialise = true;
    }
    int choix;

    do {
        printf("\n--- Menu Principal ---\n");
        printf("1. Initialiser le disque\n");
        printf("2. Créer un fichier\n");
        printf("3. Insérer un enregistrement\n");
        printf("4. Rechercher un enregistrement\n");
        printf("5. Supprimer un enregistrement\n");
        printf("6. Afficher l'état de la mémoire secondaire\n");
        printf("7. Compacter la mémoire\n");
        printf("8. Vider la mémoire secondaire\n");
        printf("9. Défragmenter le système de fichiers\n");
        printf("10. Supprimer un fichier\n");
        printf("11. Afficher les métadonnées des fichiers\n");
        printf("12. Renommer un fichier\n");  // New option
        printf("13. Quitter\n");  // Updated option number
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: {
                // Initialiser le disque
                int nbBlocs, tailleBloc;
                printf("Entrez le nombre de blocs : ");
                scanf("%d", &nbBlocs);
                printf("Entrez la taille des blocs : ");
                scanf("%d", &tailleBloc);

                if (initialiserDisque(&fs, nbBlocs, tailleBloc)) {
                    disqueInitialise = true;
                    sauvegarderAutomatiquement(&fs);
                    printf("Disque initialisé avec succès.\n");
                } else {
                    printf("Erreur lors de l'initialisation du disque.\n");
                }
                break;
            }

            case 2: {
                // Créer un fichier
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char nomFichier[MAX_FILENAME_LENGTH];
                int nbEnregistrements, organisationGlobale, organisationInterne;

                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez le nombre d'enregistrements : ");
                scanf("%d", &nbEnregistrements);
                printf("Choisissez l'organisation globale (0: Contigu, 1: Chaîné) : ");
                scanf("%d", &organisationGlobale);
                printf("Choisissez l'organisation interne (0: Trié, 1: Non trié) : ");
                scanf("%d", &organisationInterne);

                creerFichier(&fs, nomFichier, nbEnregistrements, organisationGlobale, organisationInterne);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 3: {
                // Insérer un enregistrement
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char nomFichier[MAX_FILENAME_LENGTH];
                Enregistrement enregistrement;

                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID de l'enregistrement : ");
                scanf("%d", &enregistrement.id);
                printf("Entrez les données de l'enregistrement : ");
                scanf("%s", enregistrement.info);
                enregistrement.estSupp = false;

                insererEnregistrement(&fs, nomFichier, enregistrement);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 4: {
                // Rechercher un enregistrement
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char nomFichier[MAX_FILENAME_LENGTH];
                int id;

                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID de l'enregistrement : ");
                scanf("%d", &id);

                int indexBloc = rechercherEnregistrement(&fs, nomFichier, id);
                if (indexBloc != -1) {
                    printf("Enregistrement trouvé dans le bloc %d.\n", indexBloc);
                }
                break;
            }

            case 5: {
                // Supprimer un enregistrement
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char nomFichier[MAX_FILENAME_LENGTH];
                int id, mode;

                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID de l'enregistrement : ");
                scanf("%d", &id);
                printf("Choisissez le mode de suppression (0: Logique, 1: Physique) : ");
                scanf("%d", &mode);

                supprimerEnregistrement(&fs, nomFichier, id, mode);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 6: {
                // Afficher l'état de la mémoire secondaire
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                afficherEtatMemoireSecondaire(&fs);
                break;
            }

            case 7: {
                // Compacter la mémoire
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                compacter(&fs);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 8: {
                // Vider la mémoire secondaire
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                viderMemoireSecondaire(&fs);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 9: {
                // Défragmenter le système de fichiers
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                defragmenterSystemeFichiers(&fs);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 10: {
                // Supprimer un fichier
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char nomFichier[MAX_FILENAME_LENGTH];
                printf("Entrez le nom du fichier à supprimer : ");
                scanf("%s", nomFichier);

                supprimerFichier(&fs, nomFichier);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 11: {
                // Afficher les métadonnées des fichiers
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                afficherMetadonnees(&fs);
                break;
            }

            case 12: {
                // Renommer un fichier
                if (!disqueInitialise) {
                    printf("Erreur: Le disque n'est pas initialisé.\n");
                    break;
                }

                char ancienNom[MAX_FILENAME_LENGTH];
                char nouveauNom[MAX_FILENAME_LENGTH];

                printf("Entrez le nom actuel du fichier : ");
                scanf("%s", ancienNom);
                printf("Entrez le nouveau nom du fichier : ");
                scanf("%s", nouveauNom);

                renommerFichier(&fs, ancienNom, nouveauNom);
                sauvegarderAutomatiquement(&fs);
                break;
            }

            case 13: {
                // Quitter le programme
                printf("Au revoir!\n");
                exit(0);
            }

            default: {
                printf("Choix invalide. Veuillez réessayer.\n");
                break;
            }
        }
    } while (choix != 13);

    return 0;
}

int main() {
    afficherMenu();
    return 0;
}