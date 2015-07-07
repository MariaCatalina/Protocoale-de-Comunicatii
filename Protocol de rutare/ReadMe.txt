	<Tema 2 : Simularea unui protocol de Rutare>

	 Pentru implementarea temei am folosit structuri pentru a defini tabela de
 rutare si topologia.

 -> "top_struct.h" contine declararea structurii topologie care contine numarul
 de noduri si o matrice pentru a retine legaturile dintre noduri
 -> "top_struct.cpp" implementeaza functiile definile in header:
 	init() aloca memorie pentru matrice si initializeaza cu o valore maxima
 		   fiecare pozitie
 	addNode() adauga in matrice o muchie in functie de parametri primiti
 	removeNode() sterge din matrice o muchie 
 	clear() sterge vecinii direct conectati pentru un nod
 	code() parcurge matricea si pentru fiecare legatura diferita de valoarea
 		   maxima adauga la sirVecini si adauga (sursa cost destinatie)

-> "tabela_route.h" contine declararea structurii t_route care contine numarul
de noduri si matricea de forma destinatie | nextHop | distanta
-> "tabela_route.cpp" contine implementare funtiilor definite in header
	init() aloca memorie pentru matrice si initializeaza tabela de rutare cu -1
	addRoute() adauga la tabela de rutare valorile date

-> "messages_struct.h" contine declararea structurilor pentru mesaje si evenimente
-> "messages_struct.cpp" contine implementarea functiilor din header
	addMessage() adauga un mesaj
	addEvent() adauga un eveniment

-> "sim.cpp"
	init_sim() :
		- se citesc datele din topology.in si se completaze vectorul de topology
		pentru fiecare ruter.
		- se initializeaza matricea de versiuni care contine pentru fiecare nod
		versiune primita de la celelalte noduri
		- pentru fiecare ruter se parcurg vecinii si se face floodarea initiala
		cu mesaje cu versiunea 1
		- se citesc date din fisierul messages.in si se conpleteaza vectorul de
		mesaje my_messages
		- se citesc date din fisierul events.in si se compleataza vectorul de 
		events my_events
	trigger_events()
		- pentru fiecare element din vector se verifica daca timpul simulatorului
		este egal cu timpul lui
		- in caz afirmativ se verifica daca se adauga sau se scoate o muchie
		- in ambele cazuri se apeleaza fuctiile specifice pentru nodul sursa cat
		si pentru nodul destinatie si se apeleaza functia sendNew()
	sendNew()
		- pentru nodul selectat se parcurg vecinii si se creaza mesajul dar cu
		o verciuni actualizata si se trimit tuturor vecinilor
	setTopology()
		- primeste ca parametri nodul , mesajul si lungimea lui
		- se decodeaza mesajul si se actualizeaza topologia cu noile legaturi
	Send()
		- pentru un nod i se trimite acelasi mesaj tuturor vecinilor sai direct 
		conectati
	process_messages()
		- se primesc mesajele de protocol
		- se verifica daca mesajul are o versiune mai noua si in caz afirmativ se
		proceseaza
		- se face clear la topologie pentru a sterge vechi vecini si se 
		actualizeaza apoi se face forward
		- pentru fiecare nod se verifica daca a primit un mesaj
		- daca destinatia nu e nodul curent se retrimite mesajul pentru nextHop
		- pentru fiecare mesaj din vector si daca timpul simulatoruli este egal
		cu timpul mesajului se trimite pentru nextHop determinat din tabela de 
		rutare
	dijkstra()
		- se initializeaza vectorii de distante , vecini si noduri deja selectate
		- se determina valoarea minima din vectorul de distante si pozitia lui
		- se verifica daca distanta este mai mica si se actualizeaza
		- se pune conditie si pentru distante egale pentru a lua next hop cu 
		indicele cel mai mic
		- se calculeaza distantele minime pentru un nod si se completaza tabela
		de rutare
	update_routing_table()
		- se parcurge fiecare ruter si se apeleaza functia de calcul a tabelei de
		rutare


Referinte :
	- pentru calcului distatelor minime am implementat algoritmul din curs
