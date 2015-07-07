	<Tema 4: Client HTTP>

	Programul primeste un link ca argument in linia de comanda si descarca 
pagina ceruta de utilizator.
	Descriere functii:

-> getAdress: primeste ca parametru pagina web si intoare sirul cu urmatorul
			  format: www.page.com. Acest lucru este facut prin apeluri succesive
			  ale functie strstr care cauta dupa primul si urmatorul "/".
-> extractLink: primeste in string si intoarce link-ul de forma /dir1/page.html.
				Se cauta sirul aflat intre ghilimele.	
-> verificareExtensie: se inverseaza link-ul primit si se cauta prima aparitie
					   a lui ".", apoi se vefirica dimensiunea extensieiin functie
					   de sirul nou format.
-> verificaLink: functia verifica daca sirul primit contine "<a href", "href" sau
				 "<a rel", apoi extrage link-ul din expresie si verifica daca 
				 link-ul este valid.	
-> getFileAdrdress: primeste o adresa de forma http://www.page.com/dir1/page.html.
					Extrage din sir link-ul, directorul si un vector de caractere
					care contine ierarhia de directoare prin folosirea functiei
					strstr(sir,"/"). 
-> linkPages: functia parcurge paginile, apeleaza recursiv si descarca link-urile.
			  Se verifica conditia de oprire, ca numarul recursivitatii sa fie < 6
			  Se declara variabilele necesare si se apeleaza functia getAdress 
			  pentru a deternima adresa link-ului si pentru aflarea adresei IP
			  prin aplelui functiei gethostbynama. Se formeaza adresa serverului
			  ,se face conectarea si se trimite comanda la server. Se elimina 
			  header-ul din fiecare pagina primita. 
			  -Daca s == 1 inseamna ca link-ul primit trebuie descarcat fiind de
			  forma .zip, .pdf. Se apeleaza functia recv si se scriu datele in 
			  fisier.
			  -Daca s == 0 inseamna ca pagina primita e .html. Se citeste linie 
			  cu linie si se prelucreaza. Se verifica daca string-ul contine un
			  link valid si il extrage. Se apeleaza functia getFileAddress pentru
			  a construi parametru pentru recursivitate. Se creaza ierarhia de 
			  directorare si se verifica daca link-ul a mai fost descarcat cu 
			  ajutorul functiei access. Daca -r este dat ca parametru in linia
			  de comanda se apeleaza recursiv, aceesi caz si pentru -e.
			  Se scriu in fisier datele primite.
-> main: se verifica numarul parametrilor din linia de comanda si se seteaza 
		 variabilele activeR, activeE, activeO in functie de acestia.
		 Se creeaza primul director si se apeleaza functia linkPages pentru a se
		 descarca paginile.
