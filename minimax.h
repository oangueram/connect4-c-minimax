#ifndef MINIMAX_H
#define MINIMAX_H

#include "4enratlla.h"
#include <stdint.h>

#define MIDA_TAULA 1048576 //al ser potencia de 2, l'operació modul es molt ràpida. És suficient ja que a cada torn s'avaluen entre 100000 i 400000 taulers diferents (a profunditat 8 o 10).
#define TIPUS_EXACTE 0 //el valor no es poda, és el real d'aquell node
#define TIPUS_COTA_INFERIOR 1 //provoca tall per beta
#define TIPUS_COTA_SUPERIOR 2 //cap fill supera alfa

typedef struct node {
    struct node **fills; 
    int n_fills;
    char tauler[N][N];
    double valor;
    uint64_t hash; //codi únic de cada tauler
} Node;

typedef struct entradaTT{
    uint64_t hash; //enter sense signe de 64 bits (amb un int normal de 32 bits podriem tenir hashs iguals per taulers diferents)
    int profunditat;
    double valor;
    int tipus;
} EntradaTT;

void copiarTauler(char desti[N][N], char origen[N][N]);
int calcularNumeroDeFills(char tauler[N][N]);
int fillAColumna(char tauler[N][N], int numFill);

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max);
//void creaFills(Node *pare, int nivell, int profunditat_max);
//void creaArbre(Node *arrel, int profunditat_max);
void esborraArbre(Node *arrel);

double avaluarCentre(char tauler[N][N]);
double heuristica(char tauler[N][N], int nivell);
double avaluarMinimax(Node *node, int nivell, double alpha, double beta, int profunditat_max);
int ferMinimax(char tauler[N][N], int profunditat_max);

void inicialitzarZobrist(void);

#endif