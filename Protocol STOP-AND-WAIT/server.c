#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>

#include "lib.h"

/* calculteaza paritatea unui string */
int getParity(char *str, int len){
	int parity = 0, i , j;
	for(i = 0; i < len; i++) {			
			for(j = 0; j < 8; j++)
				parity ^= (str[i] >> j) & 1;
	}

	return parity;
}

/* functie de DeCode bytes */
char deCode(char code[2]){
	int b[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i, j = 1;
	/* formare sir de biti pentru verificare */
	for (i = 3 ; i >= 0; i -- )
		b[j++] = (code[0] >> i) & 1;

	for ( i = 7;i >= 0; i --)
		b[j++] = (code[1] >> i) & 1;

	/* calculare biti de control */
	int p1[] = {3,5,7,9,11};
	int n1 = 5, par1 = 0;
	for (i = 0;i < n1;i++)
		par1 += b[p1[i]];
	par1 = par1 % 2;

	int p2[] = {3,6,7,10,11 };
	int n2 = 5, par2 = 0;
	for(i = 0;i < n2 ;i++)
		par2 += b[p2[i]];
	par2 = par2 % 2;

	int p4[] = {5,6,7,12};
	int n4 = 4,par4 = 0;
	for(i = 0;i < n4;i++)
		par4 += b[p4[i]];
	par4 = par4 % 2;

	int p8[] = {9,10,11,12};
	int n8 = 4, par8 = 0;
	for ( i = 0;i < n8;i++)
		par8 += b[p8[i]];
	par8 = par8 % 2;

	/* verificare daca exista bit gresit */
	int bitG = 0;
	if ( par1 != b[1] )
		bitG += 1;
	if( par2 != b[2])
		bitG += 2;
	if( par4 != b[4] )
		bitG += 4;
	if(par8 != b[8] )
		bitG += 8;

	/* corectare */
	if ( bitG != 0 ){
			if ( b[bitG] == 0)
				b[bitG] = 1;
			else
				b[bitG] = 0;
	}

	/* extragere caracter */
	char mesg = 0;
	char k = 0;
	for ( i = 1 ; i <= 12 ;i++){
			if( i != 1 && i != 2 && i != 4 && i != 8){
				k++;
				if(b[i] == 1) 
					mesg |= (1 << (7-(k-1)));
			}
	}
	return mesg;
}

/* funcite de codat char */
void unCode(char mesg , char *c, char *p){
	int i = 4, j = 7;
	/* setare biti in cele doar char-uri */
	if ( ((mesg >> 7) & 1) == 1)
		(*c) |= 1 << 6;
	j = 6;

	for(i = 7;i >= 0;i--){
		if ( i != 4 ){
			if ( ((mesg >> j) & 1) == 1 ){
				(*p) |= 1 << i;
			}	
		j--;
		}	
	}

	j = 1;
	int b[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
	/* construire vector de biti pentru calcularea paritatii */
	for (i = 3 ; i >= 0; i -- )
		b[j++] = ((*c) >> i) & 1;

	for ( i = 7;i >= 0; i --)
		b[j++] = ((*p) >> i) & 1;

	/* calculare biti de control */
	int p1[] = {3,5,7,9,11};
	int n1 = 5, par1 = 0;
	for (i = 0;i < n1;i++)
		par1 += b[p1[i]];
	par1 = par1 % 2;

	int p2[] = {3,6,7,10,11};
	int n2 = 5, par2 = 0;
	for(i = 0;i < n2 ;i++)
		par2 += b[p2[i]];
	par2 = par2 % 2;

	int p4[] = {5,6,7,12};
	int n4 = 4,par4 = 0;
	for(i = 0;i < n4;i++ )
		par4 += b[p4[i]];
	par4 = par4 % 2;

	int p8[] = {9,10,11,12};
	int n8 = 4, par8 = 0;
	for ( i = 0;i < n8;i++)
		par8 += b[p8[i]];
	par8 = par8 % 2;

	/* completare cu biti de paritate */
	if (par1 == 1)
			(*c) |= 1 << 3;
	if(par2 == 1)
			(*c) |= 1 << 2;
	if(par4 == 1)
			(*c) |= 1 << 0;
	if(par8 == 1)
			(*p) |= 1 << 4;

}

/* functie de decodat un mesaj */
char * totalDeCode(char *r,int len){
	int i, j = 0;
	char cmd [2];
	char *comanda = (char *)malloc(700 * sizeof(char));
	for (i = 0;i < len;i = i + 2){
		cmd[0] = r[i];
		cmd[1] = r[i+1];
		comanda[j++] = deCode(cmd);
	}
	return comanda;
}
/* functie de codat un mesaj */
msg totalUnCode(char *r){
	int i , j = 0;
	char c = 0, p = 0;
	msg m;
	memset(m.payload,0,1400);
	for(i = 0;i < strlen(r);i ++){
		unCode(r[i],&c,&p);
		m.payload[j++] = c;
		m.payload[j++] = p;
		c = 0;
		p = 0;
	}	
	m.len = j;
	return m;
}


#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc, char** args)
{
	msg r, t, m;
	struct dirent *pDirent;
	DIR *pDir;

	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	/* declarare variabile */
	char cmd[255], arg[255], sCP[1399];;	
	int lungime = 0, parity = 0, j, i, nrFiles = 0, biteWrite,readBite;
	int p;
	long lSn = 0;
	char * DirName = (char *)malloc(255 * sizeof(char));
	char * FileName = (char *)malloc(20 *sizeof (char));	
	char * buffer = (char *)malloc(1440 * sizeof(char ));
	char * Sn = (char *)malloc(1400 * sizeof(char));

while(1) {

	/* fara argumente in linia de comanda */
	if ( argc == 1){
		/* receive message */
		recv_message(&r);
		sscanf(r.payload, "%s%s", cmd, arg);

		/* comanda ls */
		if(strcmp(cmd, "ls") == 0) {
			/* send ACK */
			sprintf(t.payload,"ACK");
			t.len = strlen(t.payload) + 1;
			send_message(&t);

			pDir = opendir(arg);
			/* numarare fisere din director */
			while ( (pDirent = readdir(pDir)) != NULL){
				nrFiles ++;
			}

			closedir(pDir);
			/* send numar de fisere si astept ACK */
			sprintf(m.payload,"%d",nrFiles);
			m.len = strlen(m.payload) + 1;
			send_message(&m);		
			recv_message(&t);

			/* deschidere director pentru a trimite numele fisierelor */
			pDir = opendir(arg);

			while ( (pDirent = readdir(pDir)) != NULL){
				sprintf(m.payload,"%s",pDirent->d_name);
				m.len = strlen(m.payload);
				send_message(&m);
				recv_message(&t);
			}

			nrFiles = 0;
			closedir(pDir);
		}
		
		/* comanda cd */
		if(strcmp (cmd ,"cd") == 0){
			/* schimbare director */
			chdir(arg);
			/* send ACK */
			sprintf(t.payload,"%s","ACK");
			t.len = strlen(t.payload) + 1;
			send_message(&t);
		}
		
		/* comanda cp */
		if ( strcmp(cmd,"cp") == 0){
			/* send ACK */
			sprintf(t.payload, "ACK");
			t.len = strlen(t.payload) + 1;
			send_message(&t);
			
			/* deschidere fisier */
			FILE * fp; 
			fp = fopen (arg,"rb");

			/* calcul dimensiune fisier */
			fseek(fp, 0L, SEEK_END);
			lungime = ftell(fp);

			/* send dimendiune si astept ACK */
			sprintf(m.payload,"%d",lungime);
			m.len = strlen(m.payload);
			send_message(&m);
			recv_message(&t);
			/* pozitionare la inceput de fisier */
			fseek(fp,0L,SEEK_SET);

			/* citire din fisier si trimitere date */
			while (!feof(fp)){
				readBite = fread(m.payload,1,1400,fp);
				m.len = readBite;
				send_message(&m);
				recv_message(&t);
			}			
			fclose(fp);
			
		}
		
		/* comanda sn */
		if (strcmp(cmd,"sn") == 0){
			/* send ACK */
			sprintf(t.payload,"%s","ACK");
			t.len = strlen(t.payload) + 1;
			send_message(&t);

			/* primeste size */
			recv_message(&m);
			lSn = atol (m.payload);
			
			send_message(&t);
			/* creare nume fisier */

			strcpy(FileName,"new_");
			strcat(FileName,arg);		
			FILE * fw = fopen(FileName,"wb");

			/* primire date si scriere in fisier */
			while (lSn > 0){
				recv_message(&m);
				int biteWrite = fwrite(m.payload,1,m.len,fw);
				send_message(&t);
				lSn -= biteWrite; 
			}
			fclose(fw);
		}		
		
		/* comanda exit */
		if(strcmp(cmd,"exit") == 0){
			/* send ACK */
			sprintf(t.payload,"%s", "ACK");
			t.len = strlen(t.payload) + 1;
			send_message(&t);
			break;
		}
	}
	else
		/* argument in linia de comanda "parity" */
		if ( (argc == 2)  && (strcmp(args[1],"parity") == 0)){	
	
			/* verificare comenzi si trimitere NACK in cazul primiri lor
				 grezite */
			while(1){
					recv_message(&r);
					if (getParity(r.payload,r.len) == 0)
						break;
					sprintf(t.payload,"%s","NACK");
					t.len = 5;
					send_message(&t);
			}

			sscanf(r.payload + 1 ,"%s%s",cmd,arg);
			/* comanda ls */
			if ( strcmp (cmd,"ls") == 0){
				/* send ACK */
				strcpy(t.payload,"ACK");
				t.len = 4;
				send_message(&t);

				pDir = opendir(arg);
				nrFiles  = 0;

				/* numar directoarele din fisier */
				while ( (pDirent = readdir(pDir)) != NULL){
					nrFiles ++;
				}			
				closedir(pDir);
				/* calcul paritate pentru numarul de fisiere */
				parity = 0;
				for(j = 0; j < 16; j++)
					parity ^= (nrFiles >> j) & 1;

				sprintf(m.payload,"%d%d",parity,nrFiles);
				m.len = strlen(m.payload) + 1;

				/* trimit mesajul pana cand primesc ACK */
				while(1){
					send_message(&m);		
					recv_message(&t);
					if (t.len == 4)	
						break;
				}
				/* deschidere fisier din nou */
				pDir = opendir(arg);
			
				while ( (pDirent = readdir(pDir)) != NULL){
					/* calcul paritate mesaj si adugare la mesaj */
					p = getParity(pDirent->d_name,strlen(pDirent->d_name));
					sprintf(m.payload,"%d%s",p,pDirent->d_name);
					m.len = strlen(m.payload + 1) + 1;

					/* trimitere mesaj pana la primire NACK */
					while(1){
						send_message(&m);
						recv_message(&t);
						if (t.len == 4)
							break;
					}
				}
				closedir(pDir);
			}	
				
			/* comanda cd */
			if( strcmp(cmd,"cd") == 0){
				/* schimbare director */
				chdir(arg);
				/*send ACK */
				strcpy(t.payload,"ACK");
				t.len = 4;
				send_message(&t);
				
			}	
			/* comanda cp */
			if (strcmp(cmd,"cp") == 0){
				/* send ACK */
				strcpy(t.payload,"ACK");
				t.len = 4;
				send_message(&t);
			
				/* deschidere fisier */
				FILE * fp; 
				fp = fopen (arg,"rb");

				/* calcul lungine fisier */ 
				fseek(fp, 0L, SEEK_END);
				lungime = ftell(fp);

				/* calcul paritate lungine si adugare la mesaj */
				parity = 0;
				for(j = 0; j < 16; j++)
					parity ^= (lungime >> j) & 1;

				sprintf(m.payload,"%d%d",parity,lungime);
				m.len = strlen(m.payload) + 1;
				
				/* trimite mesaj pana cand primesc ACK */
				while (1){
					send_message(&m);
					recv_message(&t);
					if ( t.len == 4 )	
						break;
				}
				fseek(fp,0L,SEEK_SET);

				while (!feof(fp)){
					/* citire in buffer datele si calcul paritate */
					int readBite = fread(sCP ,1,1399,fp);
					int p = getParity(sCP,readBite);
					m.payload[0] = p;
					
					for(i = 0;i < readBite; i++)
						m.payload[i + 1] = sCP[i];

					m.len = readBite + 1;
					/* trimitere mesaj pana la primire ACK */
					while(1){
						send_message(&m);
						recv_message(&t);
						if (t.len == 4)
							break;
					}
			}			
		
			fclose(fp);
			}	
			
			/* comanda sn */
			if ( strcmp(cmd,"sn") == 0){
				/* send ACK */
				strcpy(t.payload,"ACK");
				t.len = 4;
				send_message(&t);
				/* asteapta mesaj pana la primire corecta */
				while (1){				
					recv_message(&m);
					if( getParity(m.payload,m.len) == 0 )
						break;
					strcpy(t.payload,"NACK");
					t.len = 5;
					send_message(&t);
				}
				/* calcul lungine fisier */
				lSn = atol (m.payload + 1);
				send_message(&t);

			  /* creare fisier */
				strcpy(FileName,"new_");
				strcat(FileName,arg);		
				FILE * fw = fopen(FileName,"wb");

				while (lSn > 0){
					/* trimitere mesaj pana cand il primeste corect */
					while(1){
						recv_message(&m);
						if( getParity(m.payload,m.len) == 0 )
							break;
						strcpy(t.payload,"NACK");
						t.len = 5;
						send_message(&t);
					}
					biteWrite = fwrite(m.payload + 1,1,m.len - 1 ,fw);
				
					/* send ACK */
					strcpy(t.payload,"ACK");
					t.len = 4;
					send_message(&t);
					/* actualizare dimensiune fisier */ 
					lSn -= biteWrite ; 
				}

				fclose(fw);
			}		
			
			/* comanda exit */
			if ( strcmp(cmd,"exit") == 0){
				/* send ACK */
				strcpy(t.payload,"ACK");
				t.len = 4;
				send_message(&t);
				break;			
			}	
	}
		else 
			/* argument in linia de comanda "hamming" */
			if( (argc == 2) && (strcmp(args[1] ,"hamming") == 0)){
				recv_message(&r);
				char *comanda = (char *)malloc(1400 * sizeof(char));
				/* decodare comanda */

				comanda = totalDeCode(r.payload,r.len);
				sscanf(comanda,"%s%s",cmd, arg);

				if(strcmp(cmd, "ls") == 0) {
					/* send ACK */
					sprintf(t.payload,"ACK");
					t.len = strlen(t.payload) + 1;
					send_message(&t);

					pDir = opendir(arg);

					nrFiles = 0;
					/* numarare fisere din director */
					while ( (pDirent = readdir(pDir)) != NULL){
						nrFiles ++;
					}

					closedir(pDir);

					/* send numar de fisere si astept ACK */
					/* apelare functie de codare */				
					sprintf(buffer,"%d",nrFiles);
					m = totalUnCode(buffer);

					send_message(&m);		
					recv_message(&t);

					pDir = opendir(arg);
					/* trimitere nume fisiere codete */
					while ( (pDirent = readdir(pDir)) != NULL){
		
						sprintf(buffer,"%s",pDirent->d_name);
						m = totalUnCode(buffer);
	
						send_message(&m);
						recv_message(&t);
					}

					closedir(pDir);
			}

			/* comanda cd */
			if(strcmp (cmd ,"cd") == 0){
				chdir(arg);
				/* send ACK */ 
				sprintf(t.payload,"%s","ACK");
				t.len = strlen(t.payload) + 1;
				send_message(&t);
			}

			/* comanda cp */
			if ( strcmp(cmd,"cp") == 0){
				/* send ACK */
				sprintf(t.payload, "ACK");
				t.len = strlen(t.payload) + 1;
				send_message(&t);
			
				/* deschidere fisier */
				FILE * fp; 
				fp = fopen (arg,"rb");

				/* calcul dimensiune fisier */
				fseek(fp, 0L, SEEK_END);
				lungime = ftell(fp);

				/* send dimendiune si astept ACK */
				sprintf(buffer,"%d",lungime);
				m = totalUnCode(buffer);

				send_message(&m);
				recv_message(&t);

				/* pozitionare la inceput de fisier */
				fseek(fp,0L,SEEK_SET);
				
				/* citire din fisier si trimitere date */
				while (!feof(fp)){

					memset(buffer,0,1400);
					fread(buffer,1,700,fp);
					m = totalUnCode(buffer);
					send_message(&m);
					recv_message(&t);
				}			
		
				fclose(fp);	
			}
			/* comanda sn */
			if (strcmp(cmd,"sn") == 0){
				/* send ACK */
				sprintf(t.payload,"%s","ACK");
				t.len = strlen(t.payload) + 1;
				send_message(&t);

				/* primeste size si il decodific */
				recv_message(&m);
				Sn = totalDeCode(m.payload,m.len);
				lSn = atol (Sn);
			
				send_message(&t);

				/* creare nume fisier */
				strcpy(FileName,"new_");
				strcat(FileName,arg);		
				FILE * fw = fopen(FileName,"wb");

				/* primire date si scriere in fisier */
				while (lSn > 0){

					recv_message(&m);
					/* decodare mesaj */
					Sn = totalDeCode(m.payload,m.len);
					biteWrite = fwrite(Sn,1,m.len/2,fw);
					send_message(&t);
					lSn -= biteWrite; 
				}
		
				fclose(fw);
			}		
		
			if(strcmp(cmd,"exit") == 0){
				/* send ACK */
				sprintf(t.payload,"%s", "ACK");
				t.len = strlen(t.payload) + 1;
				send_message(&t);
				break;
			}
				
		} 
	}	
	free(DirName);
	free(FileName);
	free(buffer);
	free(Sn);
	printf("[RECEIVER] Finished receiving..\n");
	return 0;
}
