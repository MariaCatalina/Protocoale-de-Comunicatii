/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/

#ifndef _ENDPOINT_H_
#define _ENDPOINT_H_

#include <queue>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct EpPMsg {
	int src;
	int dst;
	vector<char> message;
	int msg_size;
};

struct EpMsg {
	int src;
	int dst;
	int tag;
	string message;
};

class Endpoint {
private:
	int no_nodes;

	queue<EpMsg> *received_messages;
	queue<EpMsg> *future_received_messages;
	queue<EpPMsg> *preceived_messages;
	queue<EpPMsg> *pfuture_received_messages;

	void draw_used_path(int src, int dst, int tag, char *filename);
	void draw_used_link(int src, int dst, char *filename);
	void cat_msg_path(int tag, int src, int dst);
	void deliver_protocol_msg(Endpoint *dst, char *message, int msg_size);
	void deliver_routable_msg(Endpoint *next_hop, int dst, 
					    int tag, char *message);

public:
	int id;
	Endpoint() { }
	void init(int id, int no_nodes);
	~Endpoint();
	void swap_msgs_queues();
	void send_msg(Endpoint *dst, char *message, int msg_size, char *filename);
	void route_message(Endpoint *next_hop, int dst, int tag, char *message, char *filename);
	bool recv_message(int *src, int *dst, int *tag, char *message);
	int recv_protocol_message(char *message);
	bool are_there_any_messages_left();
};

#endif /* _ENDPOINT_H_ */
