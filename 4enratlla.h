#ifndef ENRATLLA_H
#define ENRATLLA_H

#define N 8

void inicialitzarTauler(char tauler[N][N]);
void imprimirTauler(char tauler[N][N]);
int tirada(char tauler[N][N], int columna, char fitxa);
int comprovarGuanyador(char tauler[N][N], char fitxa);
int empat(char tauler[N][N]);

char demanarTornInicial(void);
int demanarTiradaJugador(char tauler[N][N]);

// Heurística
double avaluarCentre(char tauler[N][N]);
int teSuport(char tauler[N][N], int fila, int col);
int avaluarFinestra(char c1, char c2, char c3, char c4, int *dobleamenacaO, int *dobleamenacaX, int esImmediata);
double avaluarLinies(char tauler[N][N]);
double heuristica(char tauler[N][N], int nivell);

#endif
