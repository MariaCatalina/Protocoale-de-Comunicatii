/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include "sim.h"
#include "api.h"
#include "top_struct.h"
#include "tabela_route.h"
#include "messages_struct.h"

using namespace std;

int no_node, no_m, no_ev; 
int **vers;

/* declarari globale ale vectorilor de structuri */
vector<topologie> topology;
vector<t_route> rutare;
vector<my_messages> mesaje;
vector<my_events> events;

void init_sim(int argc, char **argv) {
  string line_top, line;
  int src , dst, cost, mtime, tag, nr_unit, type;
  char * m = new char[255];
  char * mesg = new char[255];

  /* deschidere file topology.in */
  fstream file1 (argv[1],ios_base::in);
  file1 >> no_node;

  for(int i = 0 ;i < no_node ;i ++){
    topology.push_back(topologie());
    init(topology[i],no_node);
  }
  /* citire date din fisier si completare topologi */
  while ( file1 >> src){
  	file1 >> cost;
  	file1 >> dst;
  	addNode(topology[src],src,dst,cost);
    addNode(topology[dst],src,dst,cost);
  }
  file1.close();

  /* initializare matrice de versiuni */
  vers = new int *[no_node + 1];
  for(int i = 0 ; i< no_node;i++){
   vers[i] = new int[no_node + 1];
    for(int j = 0;j < no_node ;j ++)
      vers[i][j] = 0;
  }

  /* floodarea cu mesaje */
  for(int i = 0;i < no_node ;i ++){
    /* se parcurg vecinii */
    for(int j = 0 ;j < no_node;j++)
      if(topology[i].Matrix[i][j] != 1000 && topology[i].Matrix[i][j] != 0){
        /* se completaze mesajul cu numarul nodului si cu versiunea 1 pentru 
           este foodare initiala */
        sprintf(mesg,"%d %d %s%s",i,1,code(topology[i]),"\0");
        endpoint[i].send_msg(&endpoint[j],mesg,strlen(mesg) + 1,NULL);
       }      
    }  

  /* initializare strucura pentru tabela de rutare */
  for(int i = 0 ;i < no_node ;i ++){
    rutare.push_back(t_route());
    init(rutare[i],no_node);
  }

  /* citire din messages.in */
  fstream fmsg (argv[2],ios_base::in);
  fmsg >> no_m;
  /* initializare vector de mesaje */
  for(int i = 0; i < no_m; i ++){
    mesaje.push_back(my_messages());
  }
 /* citire din messages.in si completare vector de mesaje */
  for ( int i = 0; i< no_m; i++){
    fmsg >> src;
    fmsg >> dst;
    fmsg >> mtime;
    fmsg >> tag;
    getline(fmsg,line);
    copy(line.begin(),line.end(),m);
    m[line.length()] = '\n';
    m[line.length() + 1] = '\0';
    addMessage(mesaje[i],src,dst,mtime,tag,m);
  }
  fmsg.close();

  fstream fevent (argv[3],ios_base::in);
  fevent >> nr_unit;
  fevent >> no_ev;

  /* initializare vector de evenimente */
  for(int i = 0 ;i < no_ev; i ++)
    events.push_back(my_events());

  /* citire din events.in si completare vectorul de evenimente */
  for(int i = 0; i < no_ev; i ++){
    fevent >> src;
    fevent >> dst;
    fevent >> type;
    fevent >> cost;
    fevent >> mtime;
    addEvent(events[i],src,dst,type,cost,mtime);
  }
  fevent.close();

  delete[] m;
  delete[] mesg;
}

void clean_sim() {
  for(int i = 0 ;i < no_node ;i ++){
    delete[] vers[i];
  }
  delete[] vers;
  for(int i = 0 ;i < no_node ;i ++){
    for(int j = 0; j < no_node; j ++)
        delete[] topology[i].Matrix[j];
    delete[] topology[i].Matrix;
  }
  for(int i = 0 ;i < no_node ;i ++){
    for(int j = 0; j < no_node; j ++)
        delete[] rutare[i].Matrix[j];
    delete[] rutare[i].Matrix;
  }

}

/* trimitere mesaje dupa ce se actualizeaza , [de la cine] */
void sendNew(int i){
  char *mesg = new char[1000 * sizeof(char)];
  memset(mesg,0,1000);

  for(int j = 0 ;j < no_node;j++){
    if(topology[i].Matrix[i][j] != 1000 && topology[i].Matrix[i][j] != 0){
      /* se actualizeaza veriunea si se trimite tuturor vecinilor */
      sprintf(mesg,"%d %d %s%s",i,vers[i][i] + 1,code(topology[i]),"\0");
      endpoint[i].send_msg(&endpoint[j],mesg,strlen(mesg) + 1,NULL);
     }
    }
  delete[] mesg;
}

void trigger_events(){
  int t_time = get_time();
  for(int i = 0; i < no_ev ;i ++){
    if(events[i].time == t_time){
      /* se adauga a muchie pentru ambele topologi si se retrinit mesaje cu vecinii */
      if (events[i].type == 0){
        addNode(topology[events[i].src],events[i].src,events[i].dst,events[i].cost);
        addNode(topology[events[i].dst],events[i].src,events[i].dst,events[i].cost);
        sendNew(events[i].src);
        sendNew(events[i].dst);
      }
      else
        /* se sterge o muchie din ambele topologi si se trimit mesaje cu vecinii */
        if(events[i].type == 1){
          removeNode(topology[events[i].src],events[i].src,events[i].dst);
          removeNode(topology[events[i].dst],events[i].src,events[i].dst);
          sendNew(events[i].src);
          sendNew(events[i].dst);
        }
    }
  }
}

/* functie care decodeaza mesajele si completeaza topologia */
void setTipology(int j, char * mesg , int size){
    int src, dst, cost, offset = 0, node, v, x;
    /* se citesc primele 2 valori din sir care reprezinta numarul 
       nodului si vesiunea */
    sscanf(mesg,"%d%d%n",&node,&v,&x);
    mesg += x;

    sscanf(mesg,"%d%d%d%n",&src,&cost,&dst,&offset);
    while (sscanf(mesg,"%d%d%d%n",&src,&cost,&dst,&offset) == 3){
      mesg += offset;
      addNode(topology[j],src,dst,cost);
    }
}

/* trimitere forward */
void Send(int i, char* mesg){
  for(int j = 0 ;j < no_node;j++)
    if(topology[i].Matrix[i][j] != 1000 && topology[i].Matrix[i][j] != 0){
      endpoint[i].send_msg(&endpoint[j],mesg,strlen(mesg) + 1,NULL);
    }
}

void process_messages() {
  int node, version, nextHop, src, dst ,tag;
  char * mesg = new char[1000 * sizeof(char)];
  memset(mesg,0,1000);

  /* primire mesaje si completarea tipologiei cu vecinii */
  for(int i = 0;i < no_node; i++){
    while(endpoint[i].recv_protocol_message(mesg) > 0){
      sscanf(mesg,"%d%d",&node,&version);
    /* daca versiunea primita esta mai mare decat cea salvata se fac modificari */
      if(vers[i][node] < version){ 
      /* se sterge din topologie vecinii actuali ai nodului pentru a fi 
         completata cu noii vecini */
        clear(topology[i], node);
        vers[i][node] = version;
        setTipology(i,mesg,strlen(mesg));
        Send(i,mesg); 
      }
    }
  }

  /* procesare mesaje primite */
  for(int i = 0; i < no_node ; i ++){
    while(endpoint[i].recv_message(&src,&dst,&tag,mesg) > 0){
      /* daca destinatia nu e nodul curent -> se trimite mesjul pentru nextHop */
        if(i != dst){
          strcat(mesg,"\0");
          endpoint[i].route_message(&endpoint[rutare[i].Matrix[dst][1]],dst,tag,mesg,NULL);
       }
    }
  }

  /* trimitere mesaje din message.in */
  for(int i = 0;i < no_m ;i ++){
     if(mesaje[i].time == get_time()){
        nextHop = rutare[mesaje[i].src].Matrix[mesaje[i].dst][1];
        endpoint[mesaje[i].src].route_message(&endpoint[nextHop],mesaje[i].dst, mesaje[i].tag,mesaje[i].msg,NULL);
      }
    }
    delete[] mesg;
 }

/* functie pentru calculul drumului minim si completare tabela de rutare */
void dijkstra(int pk , int node, int no_node){
  int  S[no_node + 1] ,D[no_node + 1] ,V[no_node + 1];
  int min , k = -1;
 /* se fac initializari pentru vectori */
  for(int i = 0 ; i < no_node; i ++){
    S[i] = 0;
    D[i] = topology[pk].Matrix[node][i];
    if(D[i] < 1000)
      V[i] = i;
    else
      V[i] = -1;
  }

  S[node] = 1;
  D[node] = 0;

  for (int i = 0;i < no_node ;i++){
    /* se determina valoarea minima din vectorul de distante si se retine pozitia */
    min = 1000;
    for ( int j = 0; j < no_node ; j ++)
      if (S[j] == 0)
       if (D[j] < min){
          min = D[j];
          k = j;
        }
    S[k] = 1;
    for (int j = 0; j< no_node; j ++){
      /* se actualizeaza distanta daca se gaseste un drum de cost mai mic */
      if ((S[j] == 0) && ( D[k] + topology[pk].Matrix[k][j] < D[j])){
        D[j] = D[k] + topology[pk].Matrix[k][j];
        V[j] = V[k];
      }
      else 
      /* conditie pentru a determina indicele cel mai min in cazul drumuriloe egale */
        if( (S[j] == 0) && (D[k] + topology[pk].Matrix[k][j] == D[j]) && V[k] <= V[j] ){
          D[j] = D[k] + topology[pk].Matrix[k][j];
          V[j] = V[k];
    }
      }
  }
 /* pentru fiecare nod se adauga in tabla de rutare nestHop */
  for ( int i = 0; i< no_node ;i ++){
    if( i != node ){
      if (V[i] != -1){
        addRoute(rutare[node],i,i,V[V[i]],D[i]);
    }
    else
       addRoute(rutare[node],node,node,node,0);
   }
  }
}

void update_routing_table() {
  /* calcul tabela de rutare pentru fiecare ruter*/
  for(int i = 0 ; i < no_node ; i++){
    dijkstra(i,i,no_node);
  }
}
