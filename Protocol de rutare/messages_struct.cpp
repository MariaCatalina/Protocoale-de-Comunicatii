/* difinire functii din messages_struct.h */
#include <cstring>

#include "messages_struct.h"

using namespace std;

my_messages mesg;
my_events event;

/* functie de adaugare in structura de mesaje */
void addMessage(my_messages & mesg, int src,int dst,int mtime,int tag,char *msg){
	mesg.src = src;
	mesg.dst = dst;
	mesg.time = mtime;
	mesg.tag = tag;
	mesg.msg = new char[255];
	strcpy(mesg.msg,msg);
}

/* functie de adugare in structura de events */
void addEvent(my_events & event, int src,int dst, int type, int cost,int etime){
	event.src = src;
	event.dst = dst;
	event.type = type;
	event.cost = cost;
	event.time = etime;
}

