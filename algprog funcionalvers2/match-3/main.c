#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define MAX_LETRAS 9
#define MAX_OPTIONS 3
#define TAM_MATRIZ 9
#define TAM_QUADRADO 50
#define QTD_IMAGENS 6
#define TEMPO_LIMITE 120
#define MAX_POINTS 3000

typedef enum {
    TELA_MENU,
    TELA_INPUT_NOME,
    TELA_JOGO
} TelaAtual;

typedef struct {
    char nome[20];
    int pontuacao;
    int pontosPorPeca[QTD_IMAGENS];
} Jogador;

void GeraAleatorios(int matriz[TAM_MATRIZ][TAM_MATRIZ]);
void DesenhaMatriz(int matriz[TAM_MATRIZ][TAM_MATRIZ], int offsetX, int offsetY, Texture2D imagens[QTD_IMAGENS], int cursorX, int cursorY, int selecionadoX, int selecionadoY);
void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2);
int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], Jogador *jogador);
void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]);
void ProcessInput(Jogador *jogador, int *contletra, bool *nomedeentrada);
int ShowMenu(void);
void SalvarPontuacao(const Jogador *jogador);
int CalculaPontuacao(int numPecas);





int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "MATCH-3 | Algoritmo e Programacao");


    Rectangle textBox = {screenWidth / 2.0f - 197, 340, 360, 50}; // Desenha a caixa
    TelaAtual tela = TELA_MENU;
    Jogador jogador = {"", 0, {0}};
    int contletra = 0;
    bool nomedeentrada = false;

    Texture2D imagens[QTD_IMAGENS];
    for (int i = 0; i < QTD_IMAGENS; i++) {
        char path[30];
        snprintf(path, sizeof(path), "../imagens/logo_%d.png", i);
        imagens[i] = LoadTexture(path);
    }

    int matriz[TAM_MATRIZ][TAM_MATRIZ];
    GeraAleatorios(matriz);

    int cursorX = 0, cursorY = 0;
    int selecionadoX = -1, selecionadoY = -1;

    int totalWidth = TAM_MATRIZ * TAM_QUADRADO;
    int totalHeight = TAM_MATRIZ * TAM_QUADRADO;
    int offsetX = (screenWidth - totalWidth) / 2;
    int offsetY = (screenHeight - totalHeight) / 2;
    time_t inicio = time(NULL);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (tela) {
            case TELA_MENU:
                if (ShowMenu() == 0) {
                    tela = TELA_INPUT_NOME;
                } else {
                    CloseWindow();
                    return 0;
                }
                break;

            case TELA_INPUT_NOME:
                ProcessInput(&jogador, &contletra, &nomedeentrada);
                if (nomedeentrada) {
                    tela = TELA_JOGO;
                } else {
                    DrawText("MATCH-GAME:", 280, 200, 60, GREEN);
                    DrawText("DIGITE SEU NICKNAME:", 365, 300, 20, GREEN);
                    DrawRectangleRec(textBox, YELLOW);
                DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, GREEN);

                DrawText(jogador.nome, (int)textBox.x + 10, (int)textBox.y + 8, 40, GREEN);

                DrawText(TextFormat("NOME DO JOGADOR: %i/%i", contletra, MAX_LETRAS), 365, 420, 20, GREEN);
            }


                break;

              case TELA_JOGO:
               if (time(NULL) - inicio >= TEMPO_LIMITE || jogador.pontuacao >= MAX_POINTS ) {
                        DrawText(".GAME OVER!", screenWidth / 2 - 100, screenHeight / 2 - 20, 40, RED);
        SalvarPontuacao(&jogador);
    tela = TELA_MENU; // Volta ao menu ou outra tela de transição
}




                int tempoRestante = TEMPO_LIMITE - (int)(time(NULL) - inicio);

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
                            int pontos;
                            while ((pontos = VerificaCombos(matriz, &jogador))) {
                                jogador.pontuacao += CalculaPontuacao(pontos);
                                PreencheEspacos(matriz);
                            }
                        }
                        selecionadoX = -1;
                        selecionadoY = -1;
                    }
                }
                DesenhaMatriz(matriz, offsetX, offsetY, imagens, cursorX, cursorY, selecionadoX, selecionadoY);
                DrawText(TextFormat("Jogador: %s", jogador.nome), 10, 80, 20, RAYWHITE);
                   DrawText(TextFormat("Tempo: %d s", tempoRestante), 10, 60, 20, RAYWHITE);
                                   DrawText(TextFormat("Points: %d", jogador.pontuacao), 10, 100, 20, RAYWHITE);

for (int i = 0; i < QTD_IMAGENS; i++) {
    DrawTexture(imagens[i], 750, 60 + i * 65, RAYWHITE); // Desenha a imagem da peça
    DrawText(TextFormat("%d points", jogador.pontosPorPeca[i]), 830, 75 + i * 65, 30, RAYWHITE); // Mostra os pontos ao lado
}


                if (IsKeyPressed(KEY_ESCAPE)) {
                    SalvarPontuacao(&jogador);
                    tela = TELA_MENU;
                }
                break;
        }

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


void DesenhaMatriz(int matriz[TAM_MATRIZ][TAM_MATRIZ], int offsetX, int offsetY, Texture2D imagens[QTD_IMAGENS], int cursorX, int cursorY, int selecionadoX, int selecionadoY) {
    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            int x = offsetX + j * TAM_QUADRADO;
            int y = offsetY + i * TAM_QUADRADO;

            // Desenha a textura da peça
            if (matriz[i][j] != -1) {
                DrawTextureEx(imagens[matriz[i][j]], (Vector2){x, y}, 0.0f, (float)TAM_QUADRADO / imagens[matriz[i][j]].width, WHITE);
            }

            // Desenha borda padrão
            Color borderColor = DARKGRAY;

            // Realça o cursor
            if (i == cursorY && j == cursorX) {
                borderColor = GREEN;
            }

            // Realça a peça selecionada
            if (i == selecionadoY && j == selecionadoX) {
                borderColor = RED;
            }

            DrawRectangleLines(x, y, TAM_QUADRADO, TAM_QUADRADO, borderColor);
        }
    }
}


void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2) {
    int temp = matriz[y1][x1];
    matriz[y1][x1] = matriz[y2][x2];
    matriz[y2][x2] = temp;
}

int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], Jogador *jogador) {
    int encontrou = 0;
    int marcado[TAM_MATRIZ][TAM_MATRIZ] = {0};

    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ - 2; j++) {
            if (matriz[i][j] != -1 && matriz[i][j] == matriz[i][j+1] && matriz[i][j] == matriz[i][j+2]) {
                marcado[i][j] = marcado[i][j+1] = marcado[i][j+2] = 1;
                jogador->pontosPorPeca[matriz[i][j]] += 100; // Exemplo de pontuação
                jogador->pontuacao += 100;
                encontrou = 1;
            }
        }
    }

    for (int j = 0; j < TAM_MATRIZ; j++) {
        for (int i = 0; i < TAM_MATRIZ - 2; i++) {            if (matriz[i][j] == matriz[i+1][j] && matriz[i][j] == matriz[i+2][j]) {
                marcado[i][j] = marcado[i+1][j] = marcado[i+2][j] = 1;
                jogador->pontosPorPeca[matriz[i][j]] += 10;
                jogador->pontuacao += 10;
                encontrou = 1;
            }
        }
    }

    // Remove peças marcadas
    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            if (marcado[i][j]) {
                matriz[i][j] = -1;  // Marca a peça como vazia para ser preenchida depois
            }
        }
    }

    return encontrou;
}void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]) {
    for (int j = 0; j < TAM_MATRIZ; j++) {
        for (int i = TAM_MATRIZ - 1; i >= 0; i--) {
            if (matriz[i][j] == -1) {
                for (int k = i; k > 0; k--) {
                    matriz[k][j] = matriz[k - 1][j];
                }
                matriz[0][j] = rand() % QTD_IMAGENS;  // Novo valor aleatório no topo
            }
        }
    }
}
void SalvarPontuacao(const Jogador *jogador) {
    FILE *file = fopen("scoreboard.txt", "a");
    if (file) {
        fprintf(file, "Nome: %s, Pontos: %d\n", jogador->nome, jogador->pontuacao);
        fclose(file);
    }
}
int CalculaPontuacao(int numPecas) {
    if (numPecas == 3) return 100;
    if (numPecas == 4) return 200;
    return 500;
}

int ShowMenu(void) {
    int screenWidth = 1000;
    int screenHeight = 800;
    int selectedOption = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("MATCH-3 GAME", screenWidth / 2 - MeasureText("MATCH-3 GAME", 50) / 2, 140, 50, GREEN);
        const char *options[MAX_OPTIONS] = {"Iniciar", "Scoreboard", "Sair"};
        for (int i = 0; i < MAX_OPTIONS; i++) {
            if (i == selectedOption)
                DrawText(TextFormat("-> %s", options[i]), screenWidth / 2 - MeasureText(options[i], 30) / 2, 400 + 70 * i, 30, YELLOW);
            else
                DrawText(options[i], screenWidth / 2 - MeasureText(options[i], 30) / 2, 400 + 70 * i, 30, GREEN);
        }

        EndDrawing();

        if (IsKeyPressed(KEY_UP)) {
            selectedOption = (selectedOption - 1 + MAX_OPTIONS) % MAX_OPTIONS;
        } else if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % MAX_OPTIONS;
        }
        if (IsKeyPressed(KEY_ENTER)) return selectedOption;
    }

    return 2; // Default exit
}
void ProcessInput(Jogador *jogador, int *contletra, bool *nomedeentrada) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        (*contletra)--;
        if (*contletra < 0) *contletra = 0;
        jogador->nome[*contletra] = '\0';
    }

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (*contletra < MAX_LETRAS)) {
            jogador->nome[*contletra] = (char)key;
            (*contletra)++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        *nomedeentrada = true;
    }
}


