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
#define MAX_MOVIMENTOS 30
#define MAX_POINTS_POR_PECA 10

typedef enum {
    TELA_MENU,
    TELA_INPUT_NOME,
    TELA_JOGO,
    TELA_GAME_OVER,
    TELA_SCOREBOARD
} TelaAtual;

typedef struct {
    char nome[20];
    int pontosPorPeca[QTD_IMAGENS];
    int movimentos;
} Jogador;

// Funções principais
void GeraAleatorios(int matriz[TAM_MATRIZ][TAM_MATRIZ]);
void DesenhaMatriz(int matriz[TAM_MATRIZ][TAM_MATRIZ], int offsetX, int offsetY, Texture2D imagens[QTD_IMAGENS], int cursorX, int cursorY, int selecionadoX, int selecionadoY);
void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2);
int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], Jogador *jogador, int x1, int y1, int x2, int y2);
void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]);
void SalvarPontuacao(const Jogador *jogador);
void MostrarScoreboard(void);
int ShowMenu(void);
void ProcessInput(Jogador *jogador, int *contletra, bool *nomeDefinido);

// Função principal
int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "MATCH-3 | Algoritmo e Programacao");

    Texture2D imagens[QTD_IMAGENS];
    for (int i = 0; i < QTD_IMAGENS; i++) {
        char path[30];
        snprintf(path, sizeof(path), "../imagens/logo_%d.png", i);
        imagens[i] = LoadTexture(path);
    }
    Rectangle textBox = {screenWidth / 2.0f - 197, 340, 360, 50}; // Desenha a caixa
    TelaAtual tela = TELA_MENU;
    Jogador jogador = {"", {0}, MAX_MOVIMENTOS};
    int matriz[TAM_MATRIZ][TAM_MATRIZ];
    int cursorX = 0, cursorY = 0;
    int selecionadoX = -1, selecionadoY = -1;
    int contletra = 0;
    bool nomeDefinido = false;

    GeraAleatorios(matriz);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (tela) {
            case TELA_MENU:
                switch (ShowMenu()) {
                    case 0:
                        tela = TELA_INPUT_NOME;
                        break;
                    case 1:
                        tela = TELA_SCOREBOARD;
                        break;
                    case 2:
                        CloseWindow();
                        return 0;
                }
                break;

            case TELA_INPUT_NOME:
                ProcessInput(&jogador, &contletra, &nomeDefinido);
                if (nomeDefinido) {
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
                if (jogador.movimentos <= 0 || jogador.pontosPorPeca[0] >= MAX_POINTS_POR_PECA) {
                    tela = TELA_GAME_OVER;
                }

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
                            if (VerificaCombos(matriz, &jogador, selecionadoX, selecionadoY, cursorX, cursorY)) {
                                jogador.movimentos--;
                                PreencheEspacos(matriz);
                            } else {
                                TrocaPecas(matriz, selecionadoX, selecionadoY, cursorX, cursorY);
                            }
                        }
                        selecionadoX = -1;
                        selecionadoY = -1;
                    }
                }

            DesenhaMatriz(matriz, 200, 100, imagens, cursorX, cursorY, selecionadoX, selecionadoY);
                DrawText(TextFormat("Jogador: %s", jogador.nome), 10, 10, 20, RAYWHITE);
                DrawText(TextFormat("Movimentos: %d", jogador.movimentos), 10, 40, 20, RAYWHITE);
   for (int i = 0; i < QTD_IMAGENS; i++) {
    DrawTexture(imagens[i], 750, 60 + i * 65, RAYWHITE);
    DrawText(TextFormat("%d points", jogador.pontosPorPeca[i]), 830, 75 + i * 65, 30, RAYWHITE);
}



                break;



            case TELA_GAME_OVER:
                DrawText(TextFormat("GAME OVER! Nome: %s", jogador.nome), 300, 300, 30, RED);
                DrawText(TextFormat("Movimentos: %d", jogador.movimentos), 300, 350, 30, RED);
                if (IsKeyPressed(KEY_ENTER)) {
                    SalvarPontuacao(&jogador);
                    tela = TELA_MENU;
                }
                break;

            case TELA_SCOREBOARD:
                MostrarScoreboard();
                if (IsKeyPressed(KEY_ENTER)) tela = TELA_MENU;
                break;
        }

        EndDrawing();
    }

    for (int i = 0; i < QTD_IMAGENS; i++) UnloadTexture(imagens[i]);
    CloseWindow();
    return 0;
}


// Demais funções
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
            DrawTexture(imagens[matriz[i][j]], offsetX + j * TAM_QUADRADO, offsetY + i * TAM_QUADRADO, WHITE);
            DrawRectangleLines(offsetX + j * TAM_QUADRADO, offsetY + i * TAM_QUADRADO, TAM_QUADRADO, TAM_QUADRADO, (i == cursorY && j == cursorX) ? GREEN : DARKGRAY);
        }
    }
}

void TrocaPecas(int matriz[TAM_MATRIZ][TAM_MATRIZ], int x1, int y1, int x2, int y2) {
    int temp = matriz[y1][x1];
    matriz[y1][x1] = matriz[y2][x2];
    matriz[y2][x2] = temp;
}

int VerificaCombos(int matriz[TAM_MATRIZ][TAM_MATRIZ], Jogador *jogador, int x1, int y1, int x2, int y2) {
    int pontosObtidos = 0;
    int marcado[TAM_MATRIZ][TAM_MATRIZ] = {0};

    // Verifica horizontal e verticalmente a partir das peças movimentadas
    for (int i = 0; i < 2; i++) {
        int x = (i == 0) ? x1 : x2;
        int y = (i == 0) ? y1 : y2;

        // Horizontal
        int cont = 1;
        for (int j = x + 1; j < TAM_MATRIZ && matriz[y][j] == matriz[y][x]; j++) cont++;
        for (int j = x - 1; j >= 0 && matriz[y][j] == matriz[y][x]; j--) cont++;

        if (cont >= 3) {
            pontosObtidos += 10 * cont;
            jogador->pontosPorPeca[matriz[y][x]] += cont; // Atualiza pontos por tipo de peça
            for (int j = x; j < TAM_MATRIZ && matriz[y][j] == matriz[y][x]; j++) marcado[y][j] = 1;
            for (int j = x - 1; j >= 0 && matriz[y][j] == matriz[y][x]; j--) marcado[y][j] = 1;
        }

        // Vertical
        cont = 1;
        for (int i = y + 1; i < TAM_MATRIZ && matriz[i][x] == matriz[y][x]; i++) cont++;
        for (int i = y - 1; i >= 0 && matriz[i][x] == matriz[y][x]; i--) cont++;

        if (cont >= 3) {
            pontosObtidos += 10 * cont;
            jogador->pontosPorPeca[matriz[y][x]] += cont; // Atualiza pontos por tipo de peça
            for (int i = y; i < TAM_MATRIZ && matriz[i][x] == matriz[y][x]; i++) marcado[i][x] = 1;
            for (int i = y - 1; i >= 0 && matriz[i][x] == matriz[y][x]; i--) marcado[i][x] = 1;
        }
    }

    // Remove peças marcadas
    for (int i = 0; i < TAM_MATRIZ; i++) {
        for (int j = 0; j < TAM_MATRIZ; j++) {
            if (marcado[i][j]) matriz[i][j] = -1;
        }
    }

    return pontosObtidos;
}

void PreencheEspacos(int matriz[TAM_MATRIZ][TAM_MATRIZ]) {
    for (int j = 0; j < TAM_MATRIZ; j++) {
        for (int i = TAM_MATRIZ - 1; i >= 0; i--) {
            if (matriz[i][j] == -1) {
                for (int k = i; k > 0; k--) {
                    matriz[k][j] = matriz[k - 1][j];
                }
                matriz[0][j] = rand() % QTD_IMAGENS;  // Gera nova peça
            }
        }
    }
}

void SalvarPontuacao(const Jogador *jogador) {
    FILE *file = fopen("scoreboard.txt", "a");
    if (file) {
        int pontuacaoTotal = 0;
        for (int i = 0; i < QTD_IMAGENS; i++) {
            pontuacaoTotal += jogador->pontosPorPeca[i];
        }
        fprintf(file, " %s,/ %d, Movimentos: %d\n", jogador->nome, pontuacaoTotal, jogador->movimentos);
        fclose(file);
    } else {
        printf("Erro ao salvar a pontuação!\n");
    }
}
void MostrarScoreboard(void) {
    FILE *file = fopen("scoreboard.txt", "r");
    if (!file) {
        DrawText("Sem pontuações salvas!", 400, 300, 20, RAYWHITE);
        return;
    }

    char line[256];
    int y = 100;
    DrawText("SCOREBOARD", 400, 50, 30, GREEN);
    while (fgets(line, sizeof(line), file)) {
        DrawText(line, 100, y, 20, GREEN);
        y += 30;
    }
    fclose(file);
    DrawText("Pressione ENTER para voltar ao menu.", 300, 700, 20, RAYWHITE);
}


int ShowMenu(void) {
    const char *opcoes[] = {"Iniciar", "Scoreboard", "Sair"};
    int opcaoAtual = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("MENU PRINCIPAL", 400, 100, 40, GREEN);
        for (int i = 0; i < 3; i++) {
            Color cor = (i == opcaoAtual) ? YELLOW : WHITE;
            DrawText(opcoes[i], 450, 200 + i * 50, 30, cor);
        }

        EndDrawing();

        if (IsKeyPressed(KEY_UP)) opcaoAtual = (opcaoAtual - 1 + 3) % 3;
        if (IsKeyPressed(KEY_DOWN)) opcaoAtual = (opcaoAtual + 1) % 3;

        if (IsKeyPressed(KEY_ENTER)) return opcaoAtual;
    }

    return 2; // Sair por padrão
}

void ProcessInput(Jogador *jogador, int *contletra, bool *nomeDefinido) {
    if (IsKeyPressed(KEY_BACKSPACE) && *contletra > 0) {
        (*contletra)--;
        jogador->nome[*contletra] = '\0';
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && *contletra < 19) {
            jogador->nome[*contletra] = (char)key;
            (*contletra)++;
            jogador->nome[*contletra] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_ENTER) && *contletra > 0) {
        *nomeDefinido = true;
    }
}
