#ifndef FILE_H
#define FILE_H

#include "common.h"

// Prototypes de fonctions
// Créer un nouveau fichier
void creerFichier(SystemeFichiers* fs, char* nom, int nbEnregistrements, int organisationGlobale, int organisationInterne);
// Insérer un enregistrement dans un fichier
void insererEnregistrement(SystemeFichiers* fs, char* nomFichier, Enregistrement enregistrement);
// Rechercher un enregistrement par ID
int rechercherEnregistrement(SystemeFichiers* fs, char* nomFichier, int id) ;
// Supprimer un enregistrement (logique ou physique)
void supprimerEnregistrement(SystemeFichiers* fs, char* nomFichier, int id, bool estPhysique);
// Renommer un fichier
void renommerFichier(SystemeFichiers* fs, char* ancienNom, char* nouveauNom);
// Supprimer un fichier
void supprimerFichier(SystemeFichiers* fs, char* nomFichier) ;
// Défragmenter le système de fichiers
void defragmenterSystemeFichiers(SystemeFichiers* fs);
// Afficher les métadonnées des fichiers
void afficherMetadonnees(SystemeFichiers* fs) ;




#endif // FILE_H