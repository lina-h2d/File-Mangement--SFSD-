# Système de Gestion de Fichiers

Ce projet est un système de gestion de fichiers qui permet aux utilisateurs de gérer des fichiers et des enregistrements dans un environnement de mémoire secondaire simulé.
Le système prend en charge diverses opérations telles que la création de fichiers, l'insertion d'enregistrements, la recherche, la suppression, et bien plus encore. 
Il comprend une implémentation de la logique du système et deux types d'interfaces utilisateur : une interface textuelle et une interface graphique utilisant Raylib.

## Fonctionnalités

### **file.c**

Ce fichier contient les fonctions suivantes pour la gestion des fichiers :

- **creerFichier** : Crée un nouveau fichier avec des métadonnées spécifiques.
- **insererEnregistrement** : Insère un enregistrement dans un fichier spécifié.
- **rechercherEnregistrement** : Recherche un enregistrement par son ID dans un fichier.
- **supprimerEnregistrement** : Supprime un enregistrement (logiquement ou physiquement) d'un fichier.
- **renommerFichier** : Renomme un fichier existant.
- **supprimerFichier** : Supprime un fichier du système.
- **defragmenterSystemeFichiers** : Défragmente le système de fichiers pour optimiser l'espace.
- **afficherMetadonnees** : Affiche les métadonnées de tous les fichiers.

### **memory.c**

Ce fichier contient les fonctions suivantes pour la gestion de la mémoire :

- **lireTableAllocation** : Lit la table d'allocation des blocs.
- **sauvegarderTableAllocation** : Sauvegarde la table d'allocation des blocs.
- **initialiserDisque** : Initialise le disque avec un nombre spécifié de blocs et une taille de bloc.
- **lireBloc** : Lit un bloc spécifique de la mémoire secondaire.
- **ecrireBloc** : Écrit des données dans un bloc spécifique de la mémoire secondaire.
- **allouerBlocsContigus** : Alloue des blocs contigus dans la mémoire secondaire.
- **allouerBlocsChaines** : Alloue des blocs chaînés dans la mémoire secondaire.
- **libererBlocs** : Libère des blocs spécifiques dans la mémoire secondaire.
- **compacter** : Compacte la mémoire secondaire pour libérer de l'espace.
- **viderMemoireSecondaire** : Vide la mémoire secondaire en libérant tous les blocs.
- **afficherEtatMemoireSecondaire** : Affiche l'état actuel de la mémoire secondaire.

## Structure du Projet

Le projet est structuré comme suit :

- **include** : Contient les fichiers d'en-tête (`common.h`, `file.h`, `memory.h`).
- **src** : Contient les fichiers source (`file.c`, `memory.c`).
- **interface** : Contient les fichiers d'interface utilisateur (`textInterface.c`, `raylib_interface.c`).
- **Makefile** : Fichier pour la compilation du projet.
- **memory.bin** : Fichier binaire pour sauvegarder l'état du système de fichiers.

## Interface Utilisateur

### **textInterface.c** : Interface Textuelle

L'interface textuelle est une interface détaillée basée sur la console qui permet aux utilisateurs d'interagir avec le système via un menu interactif. Elle propose des options pour toutes les opérations prises en charge par la logique du system , telles que l'initialisation du disque, la création de fichiers, l'insertion d'enregistrements, et plus encore.

#### Fonctions Clés :

- **sauvegarderAutomatiquement(SystemeFichiers* fs)** :
  Cette fonction sauvegarde automatiquement l'état actuel du système de fichiers dans un fichier binaire (`memory.bin`). Elle est appelée après chaque opération pour garantir que les modifications sont persistantes.
  
- **chargerEtatSysteme(SystemeFichiers* fs)** :
  Cette fonction charge l'état du système de fichiers à partir du fichier binaire (`memory.bin`) au démarrage de l'application. Si le fichier n'existe pas, un nouveau système de fichiers est initialisé.

#### Fichier de Sauvegarde :

- **memory.bin** : 
  Ce fichier binaire est utilisé pour stocker l'état du système de fichiers. Il est créé et mis à jour automatiquement par la fonction `sauvegarderAutomatiquement`.

#### Menu Principal de l'Interface Textuelle :

--- Menu Principal ---
1. Initialiser le disque
2. Créer un fichier
3. Insérer un enregistrement
4. Rechercher un enregistrement
5. Supprimer un enregistrement
6. Afficher l'état de la mémoire secondaire
7. Compacter la mémoire
8. Vider la mémoire secondaire
9. Défragmenter le système de fichiers
10. Supprimer un fichier
11. Afficher les métadonnées des fichiers
12. Renommer un fichier
13. Quitter


L'interface graphique utilise la bibliothèque Raylib pour fournir une expérience utilisateur plus visuelle et interactive. 
Elle affiche une fenêtre avec des boutons pour chaque opération, ainsi qu'une représentation visuelle de l'état de la mémoire secondaire.

### **Fonctionnalités de l'Interface Graphique :**

#### Boutons Interactifs :
Chaque bouton correspond à une opération du système (par exemple, "Créer un fichier", "Insérer un enregistrement", etc.). Les boutons changent de couleur lorsqu'ils sont survolés ou cliqués.
#### Représentation Visuelle de la Mémoire :
L'état de la mémoire secondaire est affiché sous forme de blocs colorés. Les blocs occupés sont en rouge, tandis que les blocs libres sont en vert. Chaque bloc est étiqueté avec son numéro.
#### Capture de Sortie :
Les résultats des opérations (par exemple, les messages de succès ou d'erreur) sont capturés et affichés dans la fenêtre, offrant un retour visuel immédiat à l'utilisateur.
Menu Principal de l'Interface Graphique :

L'interface graphique propose un menu similaire à celui de l'interface textuelle, mais avec une présentation visuelle et des boutons cliquables :

--- Menu Principal ---
1. Initialiser le disque
2. Créer un fichier
3. Insérer un enregistrement
4. Rechercher un enregistrement
5. Supprimer un enregistrement
6. Afficher l'état de la mémoire secondaire
7. Compacter la mémoire
8. Vider la mémoire secondaire
9. Défragmenter le système de fichiers
10. Supprimer un fichier
11. Afficher les métadonnées des fichiers
12. Quitter
    
## Utilisation

Pour utiliser le système, compilez l'interface souhaitée.
pour compiler l'interface textuelle :
`gcc -o textInterface textInterface.c file.c memory.c -Iinclude`

Pour compiler l'interface graphique :
`make` , `make run`


Exécutez l'exécutable compilé pour démarrer l'interface et suivez les instructions à l'écran pour gérer les fichiers et les enregistrements.

## Dépendances

Raylib : Requis pour l'interface graphique. Installez-le à l'aide de votre gestionnaire de paquets ou depuis le site officiel.
Licence

## Contributeurs

- *Lina Haddad* 
- *Souad Tayebi* 
- *Rania Amoura* 
- *Melissa Ahlem Dahmouche* 




