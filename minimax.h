#ifndef MINIMAX_H
#define MINIMAX_H

#include "4enratlla.h"

typedef struct node {
    struct node **fills; 
    int n_fills;
    char tauler[N][N];
    double valor;
} Node;

void copiarTauler(char desti[N][N], char origen[N][N]);
int calcularNumeroDeFills(char tauler[N][N]);
int fillAColumna(char tauler[N][N], int numFill);

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max);
void creaFills(Node *pare, int nivell, int profunditat_max);
void creaArbre(Node *arrel, int profunditat_max);
void esborraArbre(Node *arrel);

double avaluarCentre(char tauler[N][N]);
double heuristica(char tauler[N][N], int nivell);
double avaluarMinimax(Node *node, int nivell);
int ferMinimax(char tauler[N][N], int profunditat_max);

#endif