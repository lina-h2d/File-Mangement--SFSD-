#include "raylib.h"
#include "../include/file.h"
#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define SCREEN_WIDTH 1200  
#define SCREEN_HEIGHT 800  
#define FONT_SIZE 18      
#define BUTTON_WIDTH 500  
#define BUTTON_HEIGHT 40
#define BUTTON_MARGIN 10
#define MAX_MESSAGE_LENGTH 1024
#define BLOCK_SIZE 20     
#define BLOCK_MARGIN 5    

bool DrawButton(const char* text, int x, int y, int width, int height) {
    Rectangle buttonRect = {x, y, width, height};
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), buttonRect);
    bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  
    DrawRectangleRec(buttonRect, isHovered ? BLUE : LIGHTGRAY);
    DrawText(text, x + 10, y + 10, FONT_SIZE, isHovered ? WHITE : BLACK);

    return isClicked;
}

void DisplayMessage(const char* message, int x, int y, Color color) {
    DrawText(message, x, y, FONT_SIZE, color);
}

// pour afficher la console dans l'interface
void CaptureOutput(char* buffer, int bufferSize, void (*function)(SystemeFichiers*), SystemeFichiers* fs) {
  
    FILE* tempFile = tmpfile();
    if (!tempFile) {
        snprintf(buffer, bufferSize, "Error: Unable to capture output.");
        return;
    }

    int oldStdout = dup(fileno(stdout));
    dup2(fileno(tempFile), fileno(stdout));

    function(fs);

    
    fflush(stdout);
    dup2(oldStdout, fileno(stdout));
    close(oldStdout);

    rewind(tempFile);
    fread(buffer, 1, bufferSize - 1, tempFile);
    buffer[bufferSize - 1] = '\0'; 

    fclose(tempFile);
}

// Function to draw l'etat de la meoire secondaire
void DrawMemoryState(SystemeFichiers* fs, int startX, int startY) {
    int blocksPerRow = 20; 
    int blockSizeWithMargin = BLOCK_SIZE + BLOCK_MARGIN;

    for (int i = 0; i < fs->nbBlocsTotaux; i++) {
        int row = i / blocksPerRow;
        int col = i % blocksPerRow;

        int x = startX + col * blockSizeWithMargin;
        int y = startY + row * blockSizeWithMargin;

        
        Color blockColor = fs->tableAllocationBlocs[i] ? RED : GREEN; // Occupee: Red, libre: Green
        DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, blockColor);

      
        char blockNumber[10];
        snprintf(blockNumber, sizeof(blockNumber), "%d", i);
        DrawText(blockNumber, x + 5, y + 5, 10, BLACK);
    }
}

int main() {
   
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "File Management System - Raylib Interface");
    SetTargetFPS(60);

    
    SystemeFichiers fs;
    bool disqueInitialise = false;
    char message[MAX_MESSAGE_LENGTH] = ""; 
    Color messageColor = BLACK;

    
    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

      
        DrawText("--- Menu Principal ---", 50, 20, FONT_SIZE, DARKGRAY);

        
        int buttonY = 50;
        if (DrawButton("1. Initialiser le disque", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            int nbBlocs = 100; 
            int tailleBloc = 10;
            if (initialiserDisque(&fs, nbBlocs, tailleBloc)) {
                disqueInitialise = true;
                strcpy(message, "Disque initialisé avec succès.");
                messageColor = GREEN;
            } else {
                strcpy(message, "Erreur lors de l'initialisation du disque.");
                messageColor = RED;
            }
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("2. Créer un fichier", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            char nomFichier[MAX_FILENAME_LENGTH] = "test.txt";
            int nbEnregistrements = 10;
            int organisationGlobale = 0; 
            int organisationInterne = 0; 
            creerFichier(&fs, nomFichier, nbEnregistrements, organisationGlobale, organisationInterne);
            strcpy(message, "Fichier créé avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("3. Insérer un enregistrement", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            char nomFichier[MAX_FILENAME_LENGTH] = "test.txt";
            Enregistrement enregistrement = {1, "Sample Data", false};
            insererEnregistrement(&fs, nomFichier, enregistrement);
            strcpy(message, "Enregistrement inséré avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("4. Rechercher un enregistrement", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            char nomFichier[MAX_FILENAME_LENGTH] = "test.txt";
            int id = 1;
            int indexBloc = rechercherEnregistrement(&fs, nomFichier, id);
            if (indexBloc != -1) {
                snprintf(message, sizeof(message), "Enregistrement trouvé dans le bloc %d.", indexBloc);
                messageColor = GREEN;
            } else {
                strcpy(message, "Enregistrement non trouvé.");
                messageColor = RED;
            }
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("5. Supprimer un enregistrement", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            char nomFichier[MAX_FILENAME_LENGTH] = "test.txt";
            int id = 1;
            bool estPhysique = false; // Logical deletion
            supprimerEnregistrement(&fs, nomFichier, id, estPhysique);
            strcpy(message, "Enregistrement supprimé avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("6. Afficher l’état de la mémoire secondaire", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            CaptureOutput(message, MAX_MESSAGE_LENGTH, afficherEtatMemoireSecondaire, &fs);
            messageColor = BLUE;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("7. Compacter la mémoire", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            compacter(&fs);
            strcpy(message, "Mémoire compactée avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("8. Vider la mémoire secondaire", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            viderMemoireSecondaire(&fs);
            strcpy(message, "Mémoire secondaire vidée avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("9. Défragmenter le système de fichiers", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            defragmenterSystemeFichiers(&fs);
            strcpy(message, "Système de fichiers défragmenté avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("10. Supprimer un fichier", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            char nomFichier[MAX_FILENAME_LENGTH] = "test.txt";
            supprimerFichier(&fs, nomFichier);
            strcpy(message, "Fichier supprimé avec succès.");
            messageColor = GREEN;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (disqueInitialise && DrawButton("11. Afficher les métadonnées des fichiers", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            CaptureOutput(message, MAX_MESSAGE_LENGTH, afficherMetadonnees, &fs);
            messageColor = BLUE;
        }
        buttonY += BUTTON_HEIGHT + BUTTON_MARGIN;

        if (DrawButton("12. Quitter", 50, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
            break; 
        }

        
        int memoryStartX = SCREEN_WIDTH / 2 + 50;
        int memoryStartY = 50;
        DrawText("--- État de la Mémoire Secondaire ---", memoryStartX, 20, FONT_SIZE, DARKGRAY);
        DrawMemoryState(&fs, memoryStartX, memoryStartY);

        DrawText(message, 50, SCREEN_HEIGHT - 50, FONT_SIZE, messageColor);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}