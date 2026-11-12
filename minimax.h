#ifndef MINIMAX_H
#define MINIMAX_H

#include "4enratlla.h"
#include <stdint.h>

#define MIDA_TAULA 1048576 // al ser potencia de 2, l'operació modul es molt ràpida 
#define TIPUS_EXACTE 0        // el valor no es poda, és el real d'aquell node
#define TIPUS_COTA_INFERIOR 1 // provoca tall per beta
#define TIPUS_COTA_SUPERIOR 2 // cap fill supera alfa

typedef struct node {
    struct node **fills;
    int n_fills;
    char tauler[N][N];
    double valor;
    uint64_t hash; // codi únic de cada tauler
} Node;

typedef struct entradaTT {
    uint64_t hash; // enter sense signe de 64 bits (amb un int normal de 32 bits podriem tenir hashs iguals per taulers diferents)
    int profunditat;
    double valor;
    int tipus;
} EntradaTT;

typedef struct taulaTransposicio {
    uint64_t zobrist[N][N][2];
    EntradaTT entrades[MIDA_TAULA];
} TaulaTransposicio;

typedef struct estadistiques {
    long long int nodesExplorats;
    long long int repetitsTT;
} Estadistiques;

TaulaTransposicio *crearTaulaTransposicio(void);
void netejarTaulaTransposicio(TaulaTransposicio *tt);
void alliberarTaulaTransposicio(TaulaTransposicio *tt);

void copiarTauler(char desti[N][N], char origen[N][N]);
int calcularNumeroDeFills(char tauler[N][N]);
int fillAColumna(char tauler[N][N], int numFill);

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max, TaulaTransposicio *tt);
void esborraArbre(Node *arrel);

double avaluarMinimax(Node *node, int nivell, double alpha, double beta, int profunditat_max, Estadistiques *stats, TaulaTransposicio *tt);
int ferMinimax(char tauler[N][N], int profunditat_max, TaulaTransposicio *tt);

uint64_t calcularHashTauler(char tauler[N][N], TaulaTransposicio *tt);

#endif
