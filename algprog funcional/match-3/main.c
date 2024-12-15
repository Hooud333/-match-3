#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define TAM_MATRIZ 9
#define TAM_QUADRADO 50
#define QTD_IMAGENS 6

typedef struct {
    char nome[20];
    int pontuacao;
} Jogador;

void GeraAleatorios(int matriz[TAM_MATRIZ][TAM_MATRIZ]);
void DesenhaMatriz(int matriz[TAM_MATRIZ][TAM_MATRIZ], int offsetX, int offsetY, Texture2D imagens[QTD_IMAGENS], int cursorX, int cursorY);
void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2);
int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], int *pontuacao);
void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]);

int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "MATCH-3 | Algoritmo e Programacao");
    SetTargetFPS(60);

    Texture2D imagens[QTD_IMAGENS];
    for (int i = 0; i < QTD_IMAGENS; i++) {
        char path[30];
        snprintf(path, sizeof(path), "../imagens/logo_%d.png", i);
        imagens[i] = LoadTexture(path);
    }

    Jogador jogador = {"Jogador", 0};
    int matriz[TAM_MATRIZ][TAM_MATRIZ];
    GeraAleatorios(matriz);

    int cursorX = 0, cursorY = 0;
    int selecionadoX = -1, selecionadoY = -1;

    int totalWidth = TAM_MATRIZ * TAM_QUADRADO;
    int totalHeight = TAM_MATRIZ * TAM_QUADRADO;
    int offsetX = (screenWidth - totalWidth) / 2;
    int offsetY = (screenHeight - totalHeight) / 2;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) cursorX = (cursorX + 1) % TAM_MATRIZ;
        if (IsKeyPressed(KEY_LEFT)) cursorX = (cursorX - 1 + TAM_MATRIZ) % TAM_MATRIZ;
        if (IsKeyPressed(KEY_DOWN)) cursorY = (cursorY + 1) % TAM_MATRIZ;
        if (IsKeyPressed(KEY_UP)) cursorY = (cursorY - 1 + TAM_MATRIZ) % TAM_MATRIZ;

        if (IsKeyPressed(KEY_ENTER)) {
            if (selecionadoX == -1 && selecionadoY == -1) {
                selecionadoX = cursorX;
                selecionadoY = cursorY;
            } else {
                if (abs(selecionadoX - cursorX) + abs(selecionadoY - cursorY) == 1) {
                    TrocaPecas(matriz, selecionadoX, selecionadoY, cursorX, cursorY);
                    while (VerificaCombos(matriz, &jogador.pontuacao)) {
                        PreencheEspacos(matriz);
                    }
                }
                selecionadoX = -1;
                selecionadoY = -1;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DesenhaMatriz(matriz, offsetX, offsetY, imagens, cursorX, cursorY);
        DrawText(TextFormat("Jogador: %s", jogador.nome), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Pontos: %d", jogador.pontuacao), 10, 40, 20, RAYWHITE);
        EndDrawing();
    }

    for (int i = 0; i < QTD_IMAGENS; i++) UnloadTexture(imagens[i]);
    CloseWindow();
    return 0;
}

void GeraAleatorios(int matriz[TAM_MATRIZ][TAM_MATRIZ]) {
    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            matriz[i][j] = rand() % QTD_IMAGENS;
        }
    }
}

void DesenhaMatriz(int matriz[TAM_MATRIZ][TAM_MATRIZ], int offsetX, int offsetY, Texture2D imagens[QTD_IMAGENS], int cursorX, int cursorY) {
    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            int x = offsetX + j * TAM_QUADRADO;
            int y = offsetY + i * TAM_QUADRADO;
            if (matriz[i][j] != -1) {
                DrawTextureEx(imagens[matriz[i][j]], (Vector2){x, y}, 0.0f, (float)TAM_QUADRADO / imagens[matriz[i][j]].width, WHITE);
            }
            DrawRectangleLines(x, y, TAM_QUADRADO, TAM_QUADRADO, DARKGRAY);
        }
    }
    DrawRectangleLines(offsetX + cursorX * TAM_QUADRADO, offsetY + cursorY * TAM_QUADRADO, TAM_QUADRADO, TAM_QUADRADO, GREEN);
}

void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2) {
    int temp = matriz[y1][x1];
    matriz[y1][x1] = matriz[y2][x2];
    matriz[y2][x2] = temp;
}

int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], int *pontuacao) {
    int encontrou = 0;
    int marcado[TAM_MATRIZ][TAM_MATRIZ] = {0};

    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ - 2; j++) {
            if (matriz[i][j] != -1 && matriz[i][j] == matriz[i][j+1] && matriz[i][j] == matriz[i][j+2]) {
                marcado[i][j] = marcado[i][j+1] = marcado[i][j+2] = 1;
                encontrou = 1;
            }
        }
    }

    for (int j = 0; j < TAM_MATRIZ; j++) {
        for (int i = 0; i < TAM_MATRIZ - 2; i++) {
            if (matriz[i][j] != -1 && matriz[i][j] == matriz[i+1][j] && matriz[i][j] == matriz[i+2][j]) {
                marcado[i][j] = marcado[i+1][j] = marcado[i+2][j] = 1;
                encontrou = 1;
            }
        }
    }

    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            if (marcado[i][j]) {
                matriz[i][j] = -1;
                *pontuacao += 10;
            }
        }
    }

    return encontrou;
}

void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]) {
    for (int j = 0; j < TAM_MATRIZ; j++) {
        for (int i = TAM_MATRIZ - 1; i >= 0; i--) {
            if (matriz[i][j] == -1) {
                for (int k = i; k > 0; k--) {
                    matriz[k][j] = matriz[k-1][j];
                }
                matriz[0][j] = rand() % QTD_IMAGENS;
            }
        }
    }
}
