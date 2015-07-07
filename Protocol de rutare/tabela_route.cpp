/* difinire functii din tabela_route.h */

#include "tabela_route.h"
using namespace std;

t_route tabela;
/* initializare tobela de routare */
void init(t_route & tabela, int no_node){
	tabela.no_node = no_node;
	tabela.Matrix = new int *[no_node];
	for(int i = 0 ;i < no_node ;i ++){
		tabela.Matrix[i] = new int [3];
		for (int j = 0; j < 3;j ++)
			tabela.Matrix[i][j] = -1;
	}
}

/* adugare valori in tabela de rutare */
void addRoute(t_route & tabela ,int indice, int dst, int nextHop, int dist){
	tabela.Matrix[indice][0] = dst;
	tabela.Matrix[indice][1] = nextHop;
	tabela.Matrix[indice][2] = dist;
}
