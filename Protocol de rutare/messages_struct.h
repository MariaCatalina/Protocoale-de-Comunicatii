/* structuri pentru lista de mesaje si lista de evants */

#ifndef _MESSAGES_STRUCT_H_
#define _MESSAGES_STRUCT_H_

#include <string>
using namespace std;

struct my_messages{
	int src;
	int dst;
	int time;
	int tag;
	char * msg;
};

struct my_events{
	int src;
	int dst;
	int type;
	int cost;
	int time;
};

extern my_messages mesg;
extern my_events event;

/* adaugare mesaj */
void addMessage(my_messages &, int src,int dst,int time,int tag,char * msg);

/* adaugare event */
void addEvent(my_events & , int src,int dst, int type, int cost,int time);

#endif /* _MESSAGES_STRUCT_H_ */
