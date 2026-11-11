#ifndef ENRATLLA_H
#define ENRATLLA_H

#define N 8

void inicialitzarTauler(char tauler[N][N]);
void imprimirTauler(char tauler[N][N]);
int tirada(char tauler[N][N], int columna, char fitxa);
int comprovarGuanyador(char tauler[N][N], char fitxa);
int empat(char tauler[N][N]);

#endif
