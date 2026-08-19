#include "../../includes/codexion.h"

void	cleanup_simulation(t_sim *sim)
{
	destroy_dongles(sim);
	destroy_mutexes(sim);
	free(sim->coders);
	free(sim->dongles);
	memset(sim, 0, sizeof(*sim));
}
