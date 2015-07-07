< Server de fisiere >

	Tema presune implementarea a trei protocoale STOP AND WAIT , in 
diferite moduri.

	1. Modul simplu
	
	Se primeste mesajul de la client, se extrage comanda si argumetul.

< comanda ls > Se trimite ACK pentru confirmarea comenzii si se deschide
directorul dat pentru a numara fisierele. Se trimite numarul de fisiere
si se dechide din nou directorul pentru extragerea fisierelor propriu zise, 
apoi se trimit clientului.

< comanda cd > Se schimba directorul curent prin comanda chdir si se 
trimite ACK pentru confirmarea comenzii.

<comanda cp > Se trimite ACK si se deschide fisierul trimis ca argument.
Se pozitioneaza cursorul la sfarsit de fisier pentru determinare dimensiunii.
Se trimite lungimea, se pozitioneaza cursorul la inceput de fisier pentru
citirea valorilor si se trimit datele , cate 1400 de bytes pana la sfarsitul
fisierului.

<comanda sn > Se trimite ACK si se creeaza fisierul "new_arg", unde arg 
este argumentul primit de la client. Se primeste dimensiunea si scrie in 
fisier pana cand acesta este 0.

<comanda exit > Se trimite ACK pentru confirmare si opreste programul.

	2. Modul verificare paritate
	
	Se primeste mesajul de la client, pana cand acesta are paritatea corecta
In caz contrar se trimite NACK. Se extrage comanda si argumetul.
	Functia getParity calculeaza paritatea unui mesaj prin parcurgerea fiecarui
caracter si verificare numarului de biti de 1.

<comanda ls > Se trimite ACK pentru confirmarea comenzii, se deschide directorul
pentru a calcula numarul de fisiere si se calculeaza paritatea . Se trimite 
numarul de fisiere pana cand primesc ACK. Se deschide din nou directorul si
se formeaza mesajul din paritatea si numele fisierului si trimite clientului
pana cand confirma ca este primit corect.

< comanda cd > Se schimba directorul curent prin comnada chdir si se trimite 
ACK pentru confirmarea comenzii.

<comanda cp > Se trimite ACK pentru confirmare comanda si se deschide fisierul.
Se calculeaza pariatea lungimii fisierului si se adauga la mesajul pe care 
il trimite. Se citesc din fisier care 1399 de bytes si trimit clientului
alaturi de paritate.

<comanda sn > Se trimite ACK pentru confirmare comanda si se asteapta dimensiunea
fisierului pana cand este primita corect , in fuctie da paritatea sa, in caz
contrar se trimite NACK. Se creeaza fisierul si se scrie in el datele primite.

<comanda exit > Se trimite ACK pentru confirmare si opreste programul.

	3. Modul de transmitere cu metoda hamming si corectie

	Functia deCode decodeaza cate 2 bytes. Biti din cele 2 caractere se pun in
vectorul b , de la 1 la 12 pentru calcularea bitilor de paritate. Datele se
completeaza de la 7 la 0 , pentru fiecare caracter. Se calculeaza biti de 
paritate si verifica daca exista bit gresit si il corecteaza in caz afirmativ.
Se extrage din vector, noul char de 8 biti si se returneaza.
	Functia totalDeCode primeste ca parametri un mesaj si lungimea. Parcurge 
mesajul si returneaza decodarea si corectia lui.
	Functia unCode codeaza un character. Se formeaza cei 2 bytes prin verificarea
caracterul primit, se formeaza vectorul de 12 pozitii care contine valorile
bitilor din caracterul initial. Se calculeaza biti de paritate si se completeaza
adecvat pozitiilor lor din cei 2 bytes.
	Fucntia totalUnCode primeste ca parametru un mesaj si returneaza tipul msg.
Se parcurge mesajul, se apeleaza functia unCode pentru fiecare caracter si 
se formeaza mesajul codat.
	Se primeste mesajul si se decodeaza.

< comanda ls > Se trimite ACK pentru confirmare primire comanda si se deschide
directorul pentru a numara fisierele. Se codeaza numarul de fisiere si se trimite
clientului.  

< comanda cd > Se schimba directorul curent prin comanda chdir si se trimite 
ACK pentru confirmarea comenzii.

<comanda cp > Se trimite ACK pentru confirmare ,se calculeaza dimensiunea 
fisierului, se codeaza si se trimite clientului. Se citesc cate 700 de bytes
si se codifica si se trimit.

<comanda sn > Se trimite ACK pentru confirmare comanda si se decodifica 
dimensiunea. Se scrie in fisier datele , cate 700 de bytes deoarece au fost
decodati.

<comanda exit > Se trimite ACK pentru confirmare si opreste programul.





 
  	
	
