#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minimax.h"

uint64_t taulaZobrist[N][N][2]; // assignarem a les 128 possibles tirades (64 per jugador) un codi únic. L'ultima entrada de la matriu val 0 quan el jugador és X i 1 quan és O.
EntradaTT taulaTransposicio[MIDA_TAULA]; // mida suficient per poder guardar tots els taulers de cada torn

uint64_t rand64(void) { // funció per generar numeros aleatoris de 64 bits per associar a cada possible jugada (rand() només genera entre 15 i 31 bits)
    uint64_t r = 0;
    for (int i = 0; i < 4; i++) {
        r = (r << 16) ^ (uint64_t)rand();
    }
    return r;
}

void inicialitzarZobrist(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            taulaZobrist[i][j][0] = rand64();
            taulaZobrist[i][j][1] = rand64();
        }
    }
    for (int i = 0; i < MIDA_TAULA; i++) {
        taulaTransposicio[i].hash = 0;
        taulaTransposicio[i].profunditat = -1; // és buit
        taulaTransposicio[i].tipus = 0;
        taulaTransposicio[i].valor = 0;
    }
}

uint64_t calcularHashTauler(char tauler[N][N]) {
    uint64_t h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tauler[i][j] == 'X') {
                h ^= taulaZobrist[i][j][0];
            } else if (tauler[i][j] == 'O') {
                h ^= taulaZobrist[i][j][1];
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

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max) {
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
    p->hash = pare->hash ^ taulaZobrist[fila][col][fitxaIndex];
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

void esborraArbre(Node *arrel) { // com alliberem els fills just despres de crear-los nomes ens cal alliberar l'arrel
    if (arrel == NULL) {
        return;
    }
    free(arrel);
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
        return 150; // última casella no accessible en aquell moment
    }
    if (o == 2 && buit == 2) return 10;
    if (x == 3 && buit == 1) { // Fila favorable al jugador
        if (esImmediata) {
            (*dobleamenacaX)++;
            return -800;
        }
        return -150; // última casella no accessible en aquell moment
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

double avaluarMinimax(Node *node, int nivell, double alpha, double beta, int profunditat_max, Estadistiques *stats) {
    stats->nodesExplorats++;
    if (node->n_fills == 0) {
        node->valor = heuristica(node->tauler, nivell);
        return node->valor;
    }

    // Primer consultem a la taula de transposició si el tauler actual ja ha estat consultat per estalviar càlculs
    int profunditatRestant = profunditat_max - nivell;
    uint64_t index = node->hash % MIDA_TAULA;
    EntradaTT entrada = taulaTransposicio[index];
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
            Node *fill = creaNode(node, i, nivell + 1, profunditat_max);
            double valor = avaluarMinimax(fill, nivell + 1, alpha, beta, profunditat_max, stats);
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
            Node *fill = creaNode(node, i, nivell + 1, profunditat_max);
            double valor = avaluarMinimax(fill, nivell + 1, alpha, beta, profunditat_max, stats);
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
    if (profunditatRestant >= taulaTransposicio[index].profunditat) {
        taulaTransposicio[index].hash = node->hash;
        taulaTransposicio[index].profunditat = profunditatRestant;
        taulaTransposicio[index].valor = node->valor;
        taulaTransposicio[index].tipus = tipus;
    }
    return node->valor;
}

int ferMinimax(char tauler[N][N], int profunditat_max) {
    Estadistiques stats = {0, 0};
    clock_t t_inici = clock();
    Node *arrel = malloc(sizeof(Node));
    copiarTauler(arrel->tauler, tauler);
    arrel->hash = calcularHashTauler(arrel->tauler);
    arrel->n_fills = calcularNumeroDeFills(arrel->tauler);
    double millor = -2000000;
    double alpha = -2000000;
    double beta = 2000000;
    int millorsCandidats[N];
    int numCandidats = 0;
    for (int i = 0; i < arrel->n_fills; i++) {
        Node *fill = creaNode(arrel, i, 1, profunditat_max);
        double valor = avaluarMinimax(fill, 1, alpha, beta, profunditat_max, &stats);
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
