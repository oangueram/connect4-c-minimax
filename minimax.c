 #include <stdlib.h>
#include "minimax.h"

void copiarTauler(char desti[N][N], char origen[N][N]) {
    for (int i=0;i<N;i++){
        for (int j=0;j<N;j++){
            desti[i][j]=origen[i][j];
        }
    }
    return;
}

int calcularNumeroDeFills(char tauler[N][N]){
    int count=0;
    for (int j=0;j<N;j++){
        if (tauler[0][j]=='-'){
            count++;
        }
    }
    return count;
}

int fillAColumna(char tauler[N][N], int numFill){
    int count=0;
    for (int j=0;j<N;j++){
        if (tauler[0][j]=='-'){
            if (numFill==count){
                return j;
        }
            else count++;
        }
    }
    return -1;
}

Node *creaNode(Node *pare, int numFill, int nivell, int profunditat_max) {
	Node *p=malloc(sizeof(Node));  
	//1.Copiar Tauler
	copiarTauler(p->tauler,pare->tauler);
	//2.Tirar la fitxa a la columna que toqui
	char fitxa;
    if (nivell % 2 != 0){
        fitxa='O';
    }
    else fitxa='X';
    tirada(p->tauler, fillAColumna(p->tauler, numFill), fitxa);
    if (comprovarGuanyador(p->tauler, fitxa) == 1){
        p->n_fills=0;
        p->fills=NULL;
    }
	//3.Crear l'array de fills si n'ha de tenir
	else if (nivell<profunditat_max) {
		p->n_fills=calcularNumeroDeFills(p->tauler);
		p->fills=malloc(p->n_fills * sizeof(Node *));
	}
	else {
		p->n_fills=0;
		p->fills=NULL;
	}
	return p;
}

void creaFills(Node *pare, int nivell, int profunditat_max) { //crea fills, nets... fins a profunditat_max
	for(int i=0;i<pare->n_fills;i++) {
		pare->fills[i]=creaNode(pare, i, nivell, profunditat_max );
        if (nivell<profunditat_max && pare->fills[i]->fills!= NULL){
            creaFills(pare->fills[i], nivell+1, profunditat_max);
        }
    }
}

void creaArbre(Node *arrel, int profunditat_max){
    arrel->n_fills=calcularNumeroDeFills(arrel->tauler);
    arrel->fills=malloc(arrel->n_fills * sizeof(Node *));
	creaFills(arrel, 1, profunditat_max);	
}

void esborraArbre(Node *arrel){
    if (arrel==NULL){
        return;
    }
    else if(arrel->fills!=NULL && arrel->n_fills>0){
        for (int i=0; i<arrel->n_fills; i++){
            esborraArbre(arrel->fills[i]);
        }
        free(arrel->fills);
    }
    free(arrel);
}

double avaluarCentre(char tauler[N][N]){ //com més al centre tirem la fitxa més punts.
    double puntuacio=0;
    int pesos[N]={0, 1, 2, 3, 3, 2, 1, 0};
    for (int i=3;i<N;i++) {       
        for (int j=0;j<N;j++) {     
            if (tauler[i][j]=='O') {
                puntuacio+=pesos[j];
            }
            else if (tauler[i][j]=='X') {
                puntuacio-=pesos[j];
            }
        }
    }
    return puntuacio;
}

int avaluarFinestra(char c1, char c2, char c3, char c4){
    int o=0,x=0,buit=0;
    char finestra[4]={c1,c2,c3,c4};
    for (int k=0;k<4;k++){
        if (finestra[k]=='O') o++;
        else if (finestra[k]=='X') x++;
        else if (finestra[k]=='-') buit++;
    }
    // Si tots dos jugadors tenen fitxes a la finestra, està bloquejada
    if (o>0 && x>0){
        return 0;
    }
    // Fila favorable a la màquina ('O')
    if (o==3 && buit==1) return 50;
    if (o==2 && buit==2) return 10;

    // Fila favorable al jugador ('X') - amenaça que cal evitar
    if (x==3 && buit==1) return -50;
    if (x==2 && buit==2) return -10;

    return 0;
}

double avaluarLinies(char tauler[N][N]){
    double puntuacio=0;
    // Horitzontal
    for (int i=0;i<N;i++){
        for (int j=0;j<N-3;j++){
            puntuacio+=avaluarFinestra(tauler[i][j], tauler[i][j+1], tauler[i][j+2], tauler[i][j+3]);
        }
    }
    // Vertical
    for (int i=0;i<N-3;i++){
        for (int j=0;j<N;j++){
            puntuacio+=avaluarFinestra(tauler[i][j], tauler[i+1][j], tauler[i+2][j], tauler[i+3][j]);
        }
    }
    // Diagonal dreta (descendent cap a la dreta)
    for (int i=0;i<N-3;i++){
        for (int j=0;j<N-3;j++){
            puntuacio+=avaluarFinestra(tauler[i][j], tauler[i+1][j+1], tauler[i+2][j+2], tauler[i+3][j+3]);
        }
    }
    // Diagonal esquerra (descendent cap a l'esquerra)
    for (int i=0;i<N-3;i++){
        for (int j=3;j<N;j++){
            puntuacio+=avaluarFinestra(tauler[i][j], tauler[i+1][j-1], tauler[i+2][j-2], tauler[i+3][j-3]);
        }
    }
    return puntuacio;
}

double heuristica(char tauler[N][N], int nivell){
    if(comprovarGuanyador(tauler, 'O')==1){
        return 1000000-nivell;
    }
    else if(comprovarGuanyador(tauler, 'X')==1){
        return -1000000+nivell;
    }
    double puntuacio=0; 
    //puntuacio+=avaluarCentre(tauler);
    puntuacio+=avaluarLinies(tauler);
    return puntuacio;
}

double avaluarMinimax(Node *node, int nivell){
    if(node->fills==NULL){
        node->valor=heuristica(node->tauler, nivell);
        return node->valor;
    }
    if (nivell % 2==0){
        double millor=-2000000;
        for (int i=0;i<node->n_fills;i++){
            double valor=avaluarMinimax(node->fills[i], nivell+1);
            if(valor>millor){
                millor=valor;
            }
        }
        node->valor=millor;
    }
    if (nivell % 2!=0){
        double pitjor=2000000;
        for (int i=0;i<node->n_fills;i++){
            double valor=avaluarMinimax(node->fills[i], nivell+1);
            if(valor<pitjor){
                pitjor=valor;
            }
        }
        node->valor=pitjor;
    }
    return node->valor;
}

int ferMinimax(char tauler[N][N], int profunditat_max){
    Node *arrel=malloc(sizeof(Node));
    copiarTauler(arrel->tauler, tauler);
    creaArbre(arrel, profunditat_max);
    avaluarMinimax(arrel, 0);
    int millorsCandidats[N]; //en cas d'empat de valor entre candidats, l'escollirem aleatòriament (amb mateixa prob)
    int numCandidats = 0;
    for (int i = 0; i < arrel->n_fills; i++) {
        if (arrel->fills[i]->valor == arrel->valor) {
            millorsCandidats[numCandidats] = i;
            numCandidats++;
        }
    }
    int indexAleatori = rand() % numCandidats;
    int millorFill = millorsCandidats[indexAleatori];
    int millorColumna = fillAColumna(arrel->tauler, millorFill);
    //Introdueixo el següent bloc per tapar posicions de victoria de l'humà encara que aquest ja hagi
    //guanyat si juga perfecte. La màquina es "rendia" quan detectava que l'humà havia guanyat
    //si jugava perfecte, limitant-se a llançar la fitxa a la primera columna (totes les columnes
    //tenien el mateix valor de derrota). Amb aquest bloc, mantenim les possibilitats de guanyar de
    //màquina si l'humà es despista i no aprofita l'oportunitat de victòria.
    if (arrel->valor<=-900000) {
            for (int j=0;j<N;j++) {
                char taulerDerrota[N][N];
                copiarTauler(taulerDerrota, tauler);
                if (tirada(taulerDerrota, j, 'X')==1) {
                    if (comprovarGuanyador(taulerDerrota, 'X')==1) {
                        millorColumna=j;
                        break;
                    }
                }
            }
        }
    esborraArbre(arrel);
    return millorColumna;
}