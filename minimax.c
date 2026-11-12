#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minimax.h"
#include "4enratlla.h"

uint64_t rand64(void) { // funció per generar numeros aleatoris de 64 bits per associar a cada possible jugada (rand() només genera entre 15 i 31 bits)
    uint64_t r = 0;
    for (int i = 0; i < 4; i++) {
        r = (r << 16) ^ (uint64_t)rand();
    }
    return r;
}

TaulaTransposicio *crearTaulaTransposicio(void) {
    TaulaTransposicio *tt = malloc(sizeof(TaulaTransposicio));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            tt->zobrist[i][j][0] = rand64();
            tt->zobrist[i][j][1] = rand64();
        }
    }
    netejarTaulaTransposicio(tt);
    return tt;
}

void netejarTaulaTransposicio(TaulaTransposicio *tt) {
    for (int i = 0; i < MIDA_TAULA; i++) {
        tt->entrades[i].hash = 0;
        tt->entrades[i].profunditat = -1; // és buit
        tt->entrades[i].tipus = 0;
        tt->entrades[i].valor = 0;
    }
}

void alliberarTaulaTransposicio(TaulaTransposicio *tt) {
    if (tt != NULL) {
        free(tt);
    }
}

uint64_t calcularHashTauler(char tauler[N][N], TaulaTransposicio *tt) {
    uint64_t h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tauler[i][j] == 'X') {
                h ^= tt->zobrist[i][j][0];
            } else if (tauler[i][j] == 'O') {
                h ^= tt->zobrist[i][j][1];
            }
        }
    }
    return h;
}

void copiarTauler(char desti[N][N], char origen[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            desti[i][j] = origen[i][j];
        }
    }
}

int calcularNumeroDeFills(char tauler[N][N]) {
    int count = 0;
    for (int j = 0; j < N; j++) {
        if (tauler[0][j] == '-') {
            count++;
        }
    }
    return count;
}

int fillAColumna(char tauler[N][N], int numFill) {
    int ordre[N] = {3, 4, 2, 5, 1, 6, 0, 7};
    int count = 0;
    for (int k = 0; k < N; k++) {
        int j = ordre[k];
        if (tauler[0][j] == '-') {
            if (numFill == count) {
                return j;
            } else {
                count++;
            }
        }
    }
    return -1;
}

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max, TaulaTransposicio *tt) {
    Node *p = malloc(sizeof(Node));
    // 1. Copiar Tauler
    copiarTauler(p->tauler, pare->tauler);
    // 2. Tirar la fitxa a la columna que toqui
    char fitxa;
    if (nivell % 2 != 0) {
        fitxa = 'O';
    } else {
        fitxa = 'X';
    }
    int col = fillAColumna(p->tauler, numFill);
    tirada(p->tauler, col, fitxa);
    int fila = 0;
    for (int i = 0; i < N; i++) {
        if (p->tauler[i][col] != '-') {
            fila = i;
            break;
        }
    }
    int fitxaIndex;
    if (fitxa == 'O') fitxaIndex = 1;
    else fitxaIndex = 0;
    p->hash = pare->hash ^ tt->zobrist[fila][col][fitxaIndex];
    if (comprovarGuanyador(p->tauler, fitxa) == 1) {
        p->n_fills = 0;
        p->fills = NULL;
    }
    // 3. Crear l'array de fills si n'ha de tenir
    else if (nivell < profunditat_max) {
        p->n_fills = calcularNumeroDeFills(p->tauler);
        p->fills = NULL; // com nomes creem fills de un en un i els esborrem a l'instant no cal fer un malloc de n fills
    } else {
        p->n_fills = 0;
        p->fills = NULL;
    }
    return p;
}

/* Funcions usades abans d'introduir la poda Alfa-Beta
void creaFills(Node *pare, int nivell, int profunditat_max) { //crea fills, nets... fins a profunditat_max
    for (int i = 0; i < pare->n_fills; i++) {
        pare->fills[i] = creaNode(pare, i, nivell, profunditat_max);
        if (nivell < profunditat_max && pare->fills[i]->fills != NULL) {
            creaFills(pare->fills[i], nivell + 1, profunditat_max);
        }
    }
}

void creaArbre(Node *arrel, int profunditat_max) {
    arrel->n_fills = calcularNumeroDeFills(arrel->tauler);
    arrel->fills = malloc(arrel->n_fills * sizeof(Node *));
    creaFills(arrel, 1, profunditat_max);	
}

void esborraArbre(Node *arrel) {
    if (arrel == NULL) {
        return;
    } else if (arrel->fills != NULL && arrel->n_fills > 0) {
        for (int i = 0; i < arrel->n_fills; i++) {
            esborraArbre(arrel->fills[i]);
        }
        free(arrel->fills);
    }
    free(arrel);
}
*/

void esborraArbre(Node *arrel) { // com alliberem els fills just despres de crear-los nomes ens cal alliberar l'arrel
    if (arrel == NULL) {
        return;
    }
    free(arrel);
}

double avaluarMinimax(Node *node, int nivell, double alpha, double beta, int profunditat_max, Estadistiques *stats, TaulaTransposicio *tt) {
    stats->nodesExplorats++;
    if (node->n_fills == 0) {
        node->valor = heuristica(node->tauler, nivell);
        return node->valor;
    }

    // Primer consultem a la taula de transposició si el tauler actual ja ha estat consultat per estalviar càlculs
    int profunditatRestant = profunditat_max - nivell;
    uint64_t index = node->hash % MIDA_TAULA;
    EntradaTT entrada = tt->entrades[index];
    double alpha_original = alpha; // guardem l'alpha i beta originals per saber el tipus al final
    double beta_original = beta;
    if (entrada.hash == node->hash && entrada.profunditat >= profunditatRestant) { // si el tauler ja havia estat calculat i amb la profunditat que necessitem
        if (entrada.tipus == TIPUS_EXACTE) { // si la branca no havia estat podada tenim el valor exacte del node
            node->valor = entrada.valor;
            stats->repetitsTT++;
            return entrada.valor;
        }
        if (entrada.tipus == TIPUS_COTA_INFERIOR && entrada.valor >= beta) { // si la branca havia estat podada perquè el rival tenia una jugada millor assegurada i si ara també té una jugada millor assegurada
            node->valor = entrada.valor;
            stats->repetitsTT++;
            return entrada.valor; // tallem branca
        }
        if (entrada.tipus == TIPUS_COTA_SUPERIOR && entrada.valor <= alpha) { // si el valor d'aquest node no supera l'alfa actual, la branca és inútil.
            node->valor = entrada.valor;
            stats->repetitsTT++;
            return entrada.valor; // tallem branca
        }
    }

    if (nivell % 2 == 0) { // torn màquina
        double millor = -2000000;
        for (int i = 0; i < node->n_fills; i++) {
            Node *fill = creaNode(node, i, nivell + 1, profunditat_max, tt);
            double valor = avaluarMinimax(fill, nivell + 1, alpha, beta, profunditat_max, stats, tt);
            esborraArbre(fill);
            if (valor > millor) {
                millor = valor;
            }
            if (valor > alpha) {
                alpha = valor;
            }
            if (beta < alpha) { // el rival té una millor opció, per tant no triarà aquesta branca
                break;
            }
        }
        node->valor = millor;
    }
    if (nivell % 2 != 0) { // torn rival
        double pitjor = 2000000;
        for (int i = 0; i < node->n_fills; i++) {
            Node *fill = creaNode(node, i, nivell + 1, profunditat_max, tt);
            double valor = avaluarMinimax(fill, nivell + 1, alpha, beta, profunditat_max, stats, tt);
            esborraArbre(fill);
            if (valor < pitjor) {
                pitjor = valor;
            }
            if (valor < beta) {
                beta = valor;
            }
            if (beta < alpha) { // la maquina té una millor opció, per tant no triarà aquesta branca
                break;
            }
        }
        node->valor = pitjor;
    }
    // determinem el tipus que hem obtingut
    int tipus;
    if (node->valor <= alpha_original) {
        tipus = TIPUS_COTA_SUPERIOR;
    } else if (node->valor >= beta_original) {
        tipus = TIPUS_COTA_INFERIOR;
    } else {
        tipus = TIPUS_EXACTE;
    }
    // sobreescribim a la taula de transposició només si l'analisis d'aquest torn és a més profunditat o la mateixa (d'aquesta manera renovem taulers antics i protegim anàlisis mes complets)
    if (profunditatRestant >= tt->entrades[index].profunditat) {
        tt->entrades[index].hash = node->hash;
        tt->entrades[index].profunditat = profunditatRestant;
        tt->entrades[index].valor = node->valor;
        tt->entrades[index].tipus = tipus;
    }
    return node->valor;
}

int ferMinimax(char tauler[N][N], int profunditat_max, TaulaTransposicio *tt) {
    Estadistiques stats = {0, 0};
    clock_t t_inici = clock();
    Node *arrel = malloc(sizeof(Node));
    copiarTauler(arrel->tauler, tauler);
    arrel->hash = calcularHashTauler(arrel->tauler, tt);
    arrel->n_fills = calcularNumeroDeFills(arrel->tauler);
    double millor = -2000000;
    double alpha = -2000000;
    double beta = 2000000;
    int millorsCandidats[N];
    int numCandidats = 0;
    for (int i = 0; i < arrel->n_fills; i++) {
        Node *fill = creaNode(arrel, i, 1, profunditat_max, tt);
        double valor = avaluarMinimax(fill, 1, alpha, beta, profunditat_max, &stats, tt);
        esborraArbre(fill);
        if (valor > millor) {
            millor = valor;
            numCandidats = 0;
            millorsCandidats[numCandidats++] = fillAColumna(arrel->tauler, i);
        } else if (valor == millor) {
            millorsCandidats[numCandidats++] = fillAColumna(arrel->tauler, i);
        }
        if (valor > alpha) {
            alpha = valor;
        }
    }
    int millorColumna = millorsCandidats[rand() % numCandidats];
    free(arrel);
    clock_t t_final = clock();
    double temps = (double)(t_final - t_inici) / CLOCKS_PER_SEC;
    printf("[PROFUNDITAT %i] TEMPS:%.3f | NODES EXPLORATS %lli | REUTILITZACIONS TT:%lli\n",
           profunditat_max, temps, stats.nodesExplorats, stats.repetitsTT);
    return millorColumna;
}
