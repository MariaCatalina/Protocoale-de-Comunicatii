/* difinirea structurii si functii pentru tabela de routare 
   matrice de forma :
   destinatie | nextHop | distanta
*/

#ifndef _TABELA_ROUTE_H_
#define _TABELA_ROUTE_H_

struct t_route{
	int no_node;
	int ** Matrix;
};

extern t_route tabela;

/* initializare tabela de routare */
void init(t_route &, int no_node);

/* adauga valori in tabela de rutare */
void addRoute(t_route &,int indice, int dst, int nextHop, int dist);

#endif /* _TABELA_ROUTE_H_ */
