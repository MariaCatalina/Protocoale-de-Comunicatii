#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>

#define SMTP_PORT 80
#define MAXLEN 1024

int activR = 0, activE = 0, activO = 0;
FILE * fExit;

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
  ssize_t n, rc;
  char    c, *buffer;

  buffer = vptr;

  for ( n = 1; n < maxlen; n++ ) {	
	 if ( (rc = read(sockd, &c, 1)) == 1 ) {
	   *buffer++ = c;
	   if ( c == '\n' )
		  break;
	 }
	 else if ( rc == 0 ) {
	   if ( n == 1 )
		  return 0;
	   else
	 	   break;
	 }
  	else {
	   if ( errno == EINTR )
	   	  continue;
	   return -1;
	   }
  }

  *buffer = 0;
  return n;
}

/**
 * Trimite o comanda SMTP si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care
 * trebuie sa-l trimita serverul in caz de succes (de ex. codul
 * 250). Daca raspunsul semnaleaza o eroare se iese din program.
 */
void send_command(int sockfd, char sendbuf[]) {
  char recvbuf[MAXLEN];
  int nbytes;
  char CRLF[3];
  
  CRLF[0] = 13; CRLF[1] = 10; CRLF[2] = 0;
  strcat(sendbuf, CRLF);
  write(sockfd, sendbuf, strlen(sendbuf));
  nbytes = Readline(sockfd, recvbuf, MAXLEN - 1);
  recvbuf[nbytes] = 0;
}

/* intoarce adresa de forma www.page.com */
void getAdress(char * sir,char *rez){
  int i, dim;
  char *m = (char *)malloc(MAXLEN * sizeof(char));
  char *rest = (char *)malloc(MAXLEN * sizeof(char));

  memset(m,0,MAXLEN);
  memset(rest,0,MAXLEN);

  /* se extrage http:// din link */
  sprintf(m,"%s",strstr(sir,"/") + 2);
  
  /* se extrage prima parte pana la primul / */
  strcpy(rest,strstr(m,"/"));
  dim = (int)strlen(m) - (int)strlen(rest);
  /* se copiaza adresa in functie de dimensiunea ei */
  strncpy(rez, m,dim);

  free(m);
  free(rest);
}

/* extract link dintr-un sir */
void extractLink(char * linkCurent, char * rez){
  char * m = (char *)malloc(MAXLEN* sizeof(char));
  char * p = (char *)malloc(MAXLEN* sizeof(char));
  int dim;

  memset(rez,0,MAXLEN);
  memset(m,0,MAXLEN);
  memset(p,0,MAXLEN);
  
  /* se extrage link-ul din ghilimele */
  if(strstr(linkCurent,"\"") > 0){
    sprintf(m,"%s",strstr(linkCurent,"\"") + 1);
    /* se determina sirul de la cealalta " pana la sfarsit */
    sprintf(p,"%s",strstr(m,"\""));
    dim = strlen(m) - strlen(p) ;
    /* se extrage link-ul corespunzator lungimii sale */
    strncpy(rez,m,dim);
  }
  
  free(m);
  free(p);
}

/* functia verifica daca extensia are 3 sau 4 caractere */
int verificareExtensie(char *link){
  /* inversez linkul si caut dupa primul . */
  char *b = (char *)malloc(MAXLEN * sizeof(char));
  char *e = (char *)malloc(MAXLEN * sizeof(char));
  int i, j = 0, dim = 0;
  memset(b,0,MAXLEN);
  memset(e,0,MAXLEN);
  
  /* se inverseaza link-ul primit */
  for (i = strlen(link) - 1;i >= 0;i --)
    b[j++] = link[i];
  /* se cauta pozitia primul '.' in sirul inversat */
  if(strstr(b,".") > 0)
    sprintf(e,"%s",strstr(b,"."));
  else {
    free(b);
    free(e);  
    return 0;
  }
  /* daca se gaseste se verifica dimensiunea */
  dim = strlen(link) - strlen(e);
  
  free(b);
  free(e);
  /* daca dimensiunea extensiei este buna se intoarce 1 */
  if( dim == 3 || dim == 4)
    return 1;

  return 0;
}

/* functia verifica daca link-ul este corect, dupa criteriile date */
int vefiricaLink(char* linkCurent){
  
  int ok = 0;
  char * copy = (char *)malloc(MAXLEN* sizeof(char));
  memset(copy,0,MAXLEN);

  /* verificare daca linia contine forma ceruta */
  if(strstr(linkCurent,"<a href") > 0 ){
    /* extragere page.extensie */
      extractLink(linkCurent,copy);
      /* verificare daca link-ul este corect */
      if(strstr(copy,"://") <= 0 && strstr(copy,".com") <= 0){
        /* verifica daca extensia are 3 sau 4 caractere */
        if(verificareExtensie(copy) == 1)
          ok = 1;
      }
    }
  else 
    if(strstr(linkCurent,"href=") > 0 ){ 
      /* extragere page.extensie */
      extractLink(linkCurent,copy);
       if(strstr(copy,"://") <= 0 && strstr(copy,".com") <= 0){
        /* verifica daca extensia are 3 sau 4 caractere */
         if(verificareExtensie(copy) == 1)
         ok = 1; 
      }    
    }
    else
      if(strstr(linkCurent,"<a rel=") > 0){
        /* extragere page.extensie */
        extractLink(linkCurent,copy);
        if(strstr(copy,"://") <= 0 && strstr(copy,".com") <= 0){
        /* verifica daca extensia are 3 sau 4 caractere */
          if(verificareExtensie(copy) == 1)
            ok = 1; 
        }
      }
  free(copy);
  return ok;
}

/*functia returneaza un vector de caractere ce contine ierarhia de directoare
 si calculeaza link-ul, directorul complet */
char ** getFileAddress(char *link,char* rez, char * nextPage, char * directory){
  /* declarari */
  char *token = (char *)malloc(MAXLEN * sizeof (char));
  char *m = (char *)malloc(MAXLEN * sizeof (char));
  char *newLink = (char *)malloc(MAXLEN * sizeof (char));
  char * p =(char * )malloc(MAXLEN * sizeof(char));
  char ** vD = (char **)malloc(MAXLEN * sizeof(char *));
  char * copy = (char *)malloc(MAXLEN * sizeof(char));
  int i = 0, j = 0;

  memset(token,0,MAXLEN);
  memset(m,0,MAXLEN);
  memset(newLink,0,MAXLEN);
  memset(p,0,MAXLEN);
  memset(directory,0,MAXLEN);
  memset(rez,0,MAXLEN);
  memset(nextPage,0,MAXLEN);

  /* extragere link fara http:// */
  strcpy(m,strstr(link,"//") + 2);
  strcpy(newLink,m);

  token = strtok(newLink,"/");
  while(token != NULL){
    /* extragere doar pagina cu extensie */
    if(verificareExtensie(token) == 1)
      strcpy(nextPage,token);

    token = strtok(NULL,"/");
  }

  strncpy(rez,link,strlen(link) - strlen(nextPage));
  /* extragere link care contine doar directoarele */
  strncpy(directory,m,strlen(m) - strlen(nextPage));

  /* despart directoarele dupa / si pun intr-un vector de caractere */
  sprintf(copy,"%s",directory);  
  token = strtok(copy,"/");

  while(token != NULL){
    /* alocare pentru fiecare director nou */
    vD[i] = (char *)malloc(MAXLEN * sizeof(char));
    memset(vD[i],0,MAXLEN);

    /* daca e la primul director gasit */
    if( i == 0 && strstr(token,"..") <= 0)
      sprintf(vD[i++],"%s/",token);
    else 
      /* daca nu e primul director gasit se concateneaza la cel precedent
           noua cale */
      if(i > 0 && strstr(token,"..") <= 0){
        strcat(vD[i],vD[i - 1]);
        strcat(vD[i],token);
        strcat(vD[i],"/");
        i++;
      }
      else
 /* se trateaza cazul in care avem /../ si se revine la directorul anterior */
        if(i > 0 && strstr(token,"..") > 0){
          strcat(vD[i],vD[i - 2]);
          i++;     
        }
    token = strtok(NULL,"/");
  }

  free(m);
  free(newLink);
  free(token);
  free(p);
  free(copy);

  return vD;
}

/* functia parcurge paginile, apeleaza recursiv si descarca in functie
 de parametri dati in linia de comanda */
void linkPages(char * original, char * argument,char * filePath, char * directory,int rec, int s){
  
  /* conditia de oprire */
  if (rec > 5)
    return;

  int sockfd;
  int port = SMTP_PORT;
  int rezult, i = 0, len = 0;

  struct sockaddr_in servaddr;
  struct hostent * host;

  /* declarare constante */
  char server_ip[10];
  char sendbuf[MAXLEN]; 
  char recvbuf[MAXLEN];

  char * addres = (char *)malloc(MAXLEN * sizeof(char));
  char * ipAd = (char *)malloc(MAXLEN * sizeof(char));
  char *fileApel = (char *)malloc(MAXLEN * sizeof(char));
  char *linkPrimit = (char *)malloc(MAXLEN * sizeof(char));
  char *fileNext = (char *)malloc(MAXLEN * sizeof(char));
  char *m = (char *)malloc(MAXLEN * sizeof(char));
  char *p = (char *)malloc(MAXLEN * sizeof(char));
  char *getCom = (char *)malloc(MAXLEN * sizeof(char));
  char **d = (char **)malloc(MAXLEN * sizeof(char));

  /* setare pe 0 pentru a evita erorile */
  memset(getCom,0,MAXLEN);
  memset(p,0,MAXLEN);
  memset(addres,0,MAXLEN);
  memset(ipAd,0,MAXLEN);
  memset(fileApel,0,MAXLEN);
  memset(fileNext,0,MAXLEN);
  memset(linkPrimit,0,MAXLEN);
  memset(m,0,MAXLEN);

  /* calculare adresa primita ca parametru */
  getAdress(argument,addres);
  host = gethostbyname(addres);
  /* calculare adresa IP */
  sprintf(ipAd,"%s",inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));

  strcpy(server_ip, ipAd);

  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
    if(activO == 1){
      fprintf(fExit,"Eroare la creare socket.\n");
    }
    else
      fprintf(stderr,"Eroare la creare socket.\n");
    exit(-1);
  }  

  /* formarea adresei serverului */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if (inet_aton(server_ip, &servaddr.sin_addr) <= 0 ) {
    if(activO == 1){
      fprintf(fExit, "Adresa IP invalida.\n");
    }
    else{
      fprintf(stderr, "Adresa IP invalida.\n");
    }
    exit(-1);
  }
    
  /*  conectare la server  */
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    if(activO == 1){
      fprintf(fExit, "Eroare la conectare\n");
    }
    else{
      fprintf(stderr, "Eroare la conectare\n");
    }
    exit(-1);
  }
  /* trimitere comanda la server */
  sprintf(getCom,"GET %s HTTP/1.0 \n\n",original);

  FILE * f = fopen(filePath,"wb");

  send_command(sockfd,getCom);
 
  i = 0;
  /* se elimina antetul din pagina primita */
  while((len = Readline(sockfd,recvbuf,MAXLEN-1)) > 0){
    i++;
    if(len <= 2)
      break;
      memset(recvbuf, 0, MAXLEN);
  }

  /* daca a fost trimis link de descarcare */
  if (s == 1){
    while((len = recv(sockfd, recvbuf, MAXLEN, 0)) > 0) {
      fwrite(recvbuf, sizeof(char), len, f);
      memset(recvbuf, 0, MAXLEN);
    }
  }
  /* altfel se verifica paginile .html si de scriu in fisier */
  else{
    while(Readline(sockfd, recvbuf, MAXLEN) > 0){
      /* verifica daca linia curecta contine  */
      if(vefiricaLink(recvbuf) == 1){
        extractLink(recvbuf,p);

        if(strcmp(p,"") != 0){
          sprintf(m,"%s%s",argument,p);
          memset(directory,0,MAXLEN);
          d = getFileAddress(m,linkPrimit,fileNext,directory);
          sprintf(fileApel,"%s%s",directory,fileNext);
          
          /* numai pentru recursivitate */
          if (activR == 1 && strstr(p,".html") > 0){
            i = 0; 
            /* creeaza ierarhia de directoare */
            while(d[i] != NULL){
              mkdir(d[i],S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
              i ++;
            } 
            /* verifica daca fisierul exista */
            rezult = access(fileApel,F_OK);
            /* daca nu exista se apeleaza recursiv pentru link-urile gasite */
            if(rezult != 0 && rec <= 5 )
              linkPages(m,linkPrimit,fileApel,directory,rec + 1,0);
            }
            /* daca este activ -e se apeleaza recursiv pentru a se descarca */
            if(activE == 1 && strstr(p,".html") <= 0){
              i = 0;
              /* creeaza ierarhia de directoare */ 
              while(d[i] != NULL){
                mkdir(d[i],S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                i ++;
              } 
              /* verifica daca fisierul exista */
              rezult = access(fileApel,F_OK);
              /* se apeleaza recursiv cu s = 1 pentru a intra pe coditia
                 pentru descarcare */
              if(rezult != 0 ){
                linkPages(m,linkPrimit,fileApel,directory,1,1);
              }
            } 
          }
      }
    /* scrie paginile in fisier */
    for(i = 0; i < strlen(recvbuf) ;i ++)
      fwrite(&recvbuf[i],sizeof(char),1,f);
  }
}
  free(addres);
  free(ipAd);
  free(getCom);
  free(m);
  free(linkPrimit);
  free(fileApel);
  free(p);
  free(fileNext);

  fclose(f);
  close(sockfd);
}

int main(int argc, char **argv) {

  int link = 1, i;
   /* directorul de la cere se pleaca initial */
  char *directory = (char *)malloc(MAXLEN* sizeof(char));
  char *filePath = (char *)malloc(MAXLEN * sizeof(char));
  char *linkPrimit = (char *)malloc(MAXLEN * sizeof(char));
  char *linkScriere = (char *)malloc(MAXLEN * sizeof(char));
  char * addres = (char *)malloc(MAXLEN * sizeof(char));
  char * fileLog = (char *)malloc(MAXLEN * sizeof(char));
  char *argL = (char *)malloc(MAXLEN * sizeof(char));

  memset(fileLog,0,MAXLEN);
  memset(directory,0,MAXLEN);
  memset(filePath,0,MAXLEN);
  memset(addres,0,MAXLEN);
  memset(linkPrimit,0,MAXLEN);
  memset(linkScriere,0,MAXLEN);
  memset(argL,0,MAXLEN);

  /* se concateneaza totate argumentele din linia de comanda */
   for (i = 1; i< argc ;i ++)
    strcat(argL,argv[i]);

  /* verificare ce argumente sunt active */
  if(strstr(argL,"-r") > 0){ 
    activR = 1;
  }
  if(strstr(argL,"-e") > 0){
    activE = 1;
  }
  if(strstr(argL,"-o") > 0){
    activO = 1;
  }
  /* nu sunt argumente in linia de comanda */
  if(argc == 2){
    link = 1;
  }
  /* un singur argument */
  if(argc == 3){
    link = 2;
  }
  /* 2 argumente */
  if(argc == 4){
    link = 3;
    if(activO == 1)
      sprintf(fileLog,"%s",argv[2]);
  }
  /* toate argumentele */
  if(argc == 6){
    link = 5;
    sprintf(fileLog,"%s",argv[4]);
  }
  if(activO == 1)
    fExit = fopen(fileLog,"w");

  if(argc == 5){
    if(activO == 1){
      fprintf(fExit, "Eroare numar de argumente incorecte\n");
      exit(-1);
    }
    else{
      fprintf(stderr, "Eroare numar de argumente incorecte\n");
      exit(-1);
    }
  }
  
  
  /* trimit o singura data cand nu am argumente in linia de comanda*/
  
  getFileAddress(argv[link],linkPrimit,filePath,directory);
  sprintf(linkScriere,"%s%s",directory,filePath);

  /* se creaza directorul de start din link-ul primit ca parametru */
  getAdress(linkPrimit,addres);
  mkdir(addres,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  mkdir(directory,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

 
  /* se apeleaza functia pentru a descarca paginile cerute */
  linkPages(argv[link],linkPrimit,linkScriere,directory,1,0);

  if(activO == 1)
    fclose(fExit);
  free(directory);
  free(linkPrimit);
  free(linkScriere);
  free(filePath);
  free(addres);
  free(argL);
  free(fileLog);
}
  
