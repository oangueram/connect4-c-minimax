#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "4enratlla.h"
#include "minimax.h"

#define COLOR_VERMELL "\033[1;31m" // X
#define COLOR_GROC    "\033[1;33m" // O
#define COLOR_RESET   "\033[0m"

#define PROFUNDITAT 10

void inicialitzarTauler(char tauler[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            tauler[i][j] = '-';
        }
    }
}

void imprimirTauler(char tauler[N][N]) {
    for (int i = 0; i < N; i++) {
        printf("[");
        for (int j = 0; j < N; j++) {
            if (tauler[i][j] == 'X') {
                printf(COLOR_VERMELL "X " COLOR_RESET);
            } else if (tauler[i][j] == 'O') {
                printf(COLOR_GROC "O " COLOR_RESET);
            } else {
                printf("- ");
            }
        }
        printf("]\n");
    }
    printf(" ");
    for (int i = 0; i < N; i++) {
        printf("%i ", i);
    }
    printf("\n");
}

int tirada(char tauler[N][N], int columna, char fitxa) {
    if (columna < 0 || columna >= N) {
        return 0;
    }
    for (int i = N - 1; i >= 0; i--) {
        if (tauler[i][columna] == '-') {
            tauler[i][columna] = fitxa;
            return 1;
        }
    }
    return 0;
}

int comprovarGuanyador(char tauler[N][N], char fitxa) {
    // Horitzontal
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 3; j++) {
            if (tauler[i][j] == fitxa && tauler[i][j + 1] == fitxa &&
                tauler[i][j + 2] == fitxa && tauler[i][j + 3] == fitxa) {
                return 1;
            }
        }
    }
    // Vertical
    for (int i = 0; i < N - 3; i++) {
        for (int j = 0; j < N; j++) {
            if (tauler[i][j] == fitxa && tauler[i + 1][j] == fitxa &&
                tauler[i + 2][j] == fitxa && tauler[i + 3][j] == fitxa) {
                return 1;
            }
        }
    }
    // Diagonal dreta
    for (int i = 0; i < N - 3; i++) {
        for (int j = 0; j < N - 3; j++) {
            if (tauler[i][j] == fitxa && tauler[i + 1][j + 1] == fitxa &&
                tauler[i + 2][j + 2] == fitxa && tauler[i + 3][j + 3] == fitxa) {
                return 1;
            }
        }
    }
    // Diagonal esquerra
    for (int i = 0; i < N - 3; i++) {
        for (int j = 3; j < N; j++) {
            if (tauler[i][j] == fitxa && tauler[i + 1][j - 1] == fitxa &&
                tauler[i + 2][j - 2] == fitxa && tauler[i + 3][j - 3] == fitxa) {
                return 1;
            }
        }
    }
    return 0;
}

int empat(char tauler[N][N]) { // tauler ple
    for (int j = 0; j < N; j++) {
        if (tauler[0][j] == '-') {
            return 0;
        }
    }
    return 1;
}

char demanarTornInicial(void) {
    int comencar;
    printf("Qui tira primer? (1=Jugador, 2=Maquina)\n");
    scanf("%i", &comencar);
    while (comencar != 1 && comencar != 2) {
        printf("Opcio no valida, torna-ho a intentar.\n");
        scanf("%i", &comencar);
    }
    if (comencar == 1) {
        return 'X';
    } else {
        return 'O';
    }
}

int demanarTiradaJugador(char tauler[N][N]) {
    int columna;
    int tiradaValida;
    printf("A quina columna vols tirar?\n");
    scanf("%i", &columna);
    tiradaValida = tirada(tauler, columna, 'X');
    while (!tiradaValida) {
        printf("Columna no disponible, torna-ho a intentar.\n");
        scanf("%i", &columna);
        tiradaValida = tirada(tauler, columna, 'X');
    }
    return columna;
}

double avaluarCentre(char tauler[N][N]) { // com més al centre tirem la fitxa més punts.
    double puntuacio = 0;
    int pesos[N] = {0, 1, 2, 3, 3, 2, 1, 0};
    for (int i = 3; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tauler[i][j] == 'O') {
                puntuacio += pesos[j];
            } else if (tauler[i][j] == 'X') {
                puntuacio -= pesos[j];
            }
        }
    }
    return puntuacio;
}

int teSuport(char tauler[N][N], int fila, int col) { // funció per saber si una casella té suport o si caurà més avall per gravetat
    if (fila == N - 1) return 1;
    return (tauler[fila + 1][col] != '-');
}

int avaluarFinestra(char c1, char c2, char c3, char c4, int *dobleamenacaO, int *dobleamenacaX, int esImmediata) {
    int o = 0, x = 0, buit = 0;
    char finestra[4] = {c1, c2, c3, c4};
    for (int k = 0; k < 4; k++) {
        if (finestra[k] == 'O') o++;
        else if (finestra[k] == 'X') x++;
        else if (finestra[k] == '-') buit++;
    }
    // Si tots dos jugadors tenen fitxes a la finestra, està bloquejada
    if (o > 0 && x > 0) {
        return 0;
    }
    if (o == 3 && buit == 1) { // Fila favorable a la màquina
        if (esImmediata) {
            (*dobleamenacaO)++;
            return 800;
        }
        return 150; // Última casella no accessible en aquell moment
    }
    if (o == 2 && buit == 2) return 10;
    if (x == 3 && buit == 1) { // Fila favorable al jugador
        if (esImmediata) {
            (*dobleamenacaX)++;
            return -800;
        }
        return -150; // Última casella no accessible en aquell moment
    }
    if (x == 2 && buit == 2) return -10;

    return 0;
}

double avaluarLinies(char tauler[N][N]) {
    double puntuacio = 0;
    int dobleamenacaO = 0;
    int dobleamenacaX = 0;
    // Horitzontal
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 3; j++) {
            int esImmediata = 0;
            for (int k = 0; k < 4; k++) {
                if (tauler[i][j + k] == '-' && teSuport(tauler, i, j + k)) esImmediata = 1;
            }
            puntuacio += avaluarFinestra(tauler[i][j], tauler[i][j + 1], tauler[i][j + 2], tauler[i][j + 3], &dobleamenacaO, &dobleamenacaX, esImmediata);
        }
    }
    // Vertical
    for (int i = 0; i < N - 3; i++) {
        for (int j = 0; j < N; j++) {
            puntuacio += avaluarFinestra(tauler[i][j], tauler[i + 1][j], tauler[i + 2][j], tauler[i + 3][j], &dobleamenacaO, &dobleamenacaX, 1);
        }
    }
    // Diagonal dreta (descendent cap a la dreta)
    for (int i = 0; i < N - 3; i++) {
        for (int j = 0; j < N - 3; j++) {
            int esImmediata = 0;
            for (int k = 0; k < 4; k++) {
                if (tauler[i + k][j + k] == '-' && teSuport(tauler, i + k, j + k)) esImmediata = 1;
            }
            puntuacio += avaluarFinestra(tauler[i][j], tauler[i + 1][j + 1], tauler[i + 2][j + 2], tauler[i + 3][j + 3], &dobleamenacaO, &dobleamenacaX, esImmediata);
        }
    }
    // Diagonal esquerra (descendent cap a l'esquerra)
    for (int i = 0; i < N - 3; i++) {
        for (int j = 3; j < N; j++) {
            int esImmediata = 0;
            for (int k = 0; k < 4; k++) {
                if (tauler[i + k][j - k] == '-' && teSuport(tauler, i + k, j - k)) esImmediata = 1;
            }
            puntuacio += avaluarFinestra(tauler[i][j], tauler[i + 1][j - 1], tauler[i + 2][j - 2], tauler[i + 3][j - 3], &dobleamenacaO, &dobleamenacaX, esImmediata);
        }
    }
    if (dobleamenacaO >= 2) puntuacio += 5000;
    if (dobleamenacaX >= 2) puntuacio -= 5000;
    return puntuacio;
}

double heuristica(char tauler[N][N], int nivell) {
    if (comprovarGuanyador(tauler, 'O') == 1) {
        return 1000000 - nivell;
    } else if (comprovarGuanyador(tauler, 'X') == 1) {
        return -1000000 + nivell;
    }
    double puntuacio = 0;
    puntuacio += avaluarCentre(tauler);
    puntuacio += avaluarLinies(tauler);
    return puntuacio;
}

int main(void) {
    srand(time(NULL));
    TaulaTransposicio *tt = crearTaulaTransposicio();
    char tauler[N][N];
    int fiPartida = 0;
    char jugadorActual = demanarTornInicial();

    inicialitzarTauler(tauler);

    while (!fiPartida) {
        imprimirTauler(tauler);
        if (jugadorActual == 'X') {
            demanarTiradaJugador(tauler);
        }
        if (jugadorActual == 'O') {
            printf("Torn de la màquina.\n");
            int columna = ferMinimax(tauler, PROFUNDITAT, tt);
            tirada(tauler, columna, 'O');
            printf("La màquina ha tirat a la columna %i\n", columna);
        }
        if (comprovarGuanyador(tauler, jugadorActual) == 1) {
            imprimirTauler(tauler);
            printf("4 EN RATLLA! Ha guanyat el jugador %c!\n", jugadorActual);
            fiPartida = 1;
        } else if (empat(tauler) == 1) {
            imprimirTauler(tauler);
            printf("Empat! No queden posicions lliures.\n");
            fiPartida = 1;
        }
        if (jugadorActual == 'X') {
            jugadorActual = 'O';
        } else {
            jugadorActual = 'X';
        }
    }
    alliberarTaulaTransposicio(tt);
    return 0;
}
