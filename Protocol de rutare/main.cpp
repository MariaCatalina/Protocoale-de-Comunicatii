/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/

#include "api.h"
#include "sim.h"
#include "utils.h"

int main(int argc, char **argv)
{
	init_api(argc, argv);
	init_sim(argc, argv);

	loop {
		trigger_events();
		process_messages();
		update_routing_table();
		api_update_state();
		if (api_simulation_ended())
			break;
	}

	clean_sim();
	clean_api();

	return 0;
}
