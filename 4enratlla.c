#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "4enratlla.h"
#include "minimax.h"
#define COLOR_VERMELL "\033[1;31m"   // X
#define COLOR_GROC    "\033[1;33m"   // O 
#define COLOR_RESET   "\033[0m"      

#define PROFUNDITAT 10

void inicialitzarTauler(char tauler[N][N]){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            tauler[i][j]='-';
        }
    }
    return;
}

void imprimirTauler(char tauler[N][N]){
    for(int i = 0; i < N; i++){
        printf("[");
        for(int j = 0; j < N; j++){
            if (tauler[i][j] == 'X') {
                printf(COLOR_VERMELL "X " COLOR_RESET);
            }
            else if (tauler[i][j] == 'O') {
                printf(COLOR_GROC "O " COLOR_RESET);
            }
            else {
                printf("- ");
            }
        }
        printf("]\n");
    }
    printf(" ");
    for(int i = 0; i < N; i++){
        printf("%i ", i);
    }
    printf("\n");
}

int tirada(char tauler[N][N], int columna, char fitxa){
    if (columna<0 || columna>=N){
        return 0;
    }
    for(int i=N-1; i>=0;i--){
        if(tauler[i][columna]=='-'){
            tauler[i][columna]=fitxa;
            return 1;
        }
    }
    return 0;
}

int comprovarGuanyador(char tauler[N][N], char fitxa){
    //horitzontal
    for(int i=0;i<N;i++){
        for(int j=0;j<N-3;j++){
            if(tauler[i][j]==fitxa && tauler[i][j+1]==fitxa && tauler[i][j+2]==fitxa && tauler[i][j+3]==fitxa){
                return 1;
            }
        }
    }
    //vertical
    for(int i=0;i<N-3;i++){
        for(int j=0;j<N;j++){
            if(tauler[i][j]==fitxa && tauler[i+1][j]==fitxa && tauler[i+2][j]==fitxa && tauler[i+3][j]==fitxa){
                return 1;
            }
        }
    }
    //diagonal dreta
    for(int i=0;i<N-3;i++){
        for(int j=0;j<N-3;j++){
            if(tauler[i][j]==fitxa && tauler[i+1][j+1]==fitxa && tauler[i+2][j+2]==fitxa && tauler[i+3][j+3]==fitxa){
                return 1;
            }
        }
    }
    //diagonal esquerra
    for(int i=0;i<N-3;i++){
        for(int j=3;j<N;j++){
            if(tauler[i][j]==fitxa && tauler[i+1][j-1]==fitxa && tauler[i+2][j-2]==fitxa && tauler[i+3][j-3]==fitxa){
                return 1;
            }
        }
    }
    return 0;
}

int empat(char tauler[N][N]){ //tauler ple
    for (int j=0;j<N;j++){
        if (tauler[0][j]=='-'){
            return 0;
        }
    }
    return 1;
}

int main(){
    srand(time(NULL));
    char tauler[N][N];
    char jugadorActual='X';
    int fiPartida=0;
    int columna;
    int tiradaValida=0;
    inicialitzarTauler(tauler);
    while(!fiPartida){
        imprimirTauler(tauler);
        if (jugadorActual=='X'){  
            printf("A quina columna vols tirar?\n");
            scanf("%i", &columna);
            tiradaValida=tirada(tauler, columna, jugadorActual);
            while(!tiradaValida){
                printf("Columna no disponible, torna-ho a intentar.\n");
                scanf("%i", &columna);
                tiradaValida=tirada(tauler, columna, jugadorActual);
            }
        }
        if (jugadorActual=='O'){
            printf("Torn de la màquina.\n");
            columna = ferMinimax(tauler, PROFUNDITAT);
            tirada(tauler, columna, 'O');
            printf("La màquina ha tirat a la columna %i\n", columna);
        }
        if(comprovarGuanyador(tauler, jugadorActual)==1){
            imprimirTauler(tauler);
            printf("4 EN RATLLA! Ha guanyat el jugador %c!\n", jugadorActual);
            fiPartida=1;
        }
        else if(empat(tauler)==1){
            imprimirTauler(tauler);
            printf("Empat! No queden posicions lliures.\n");
            fiPartida=1;
        }
        if (jugadorActual=='X'){
            jugadorActual='O';
        }
        else {
            jugadorActual='X';
        }
    }
    return 0;
}