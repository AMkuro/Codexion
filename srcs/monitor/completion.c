#include "../../includes/codexion.h"

int	all_coders_completed(t_sim *sim)
{
	int	i;
	int	compile_count;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		compile_count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (compile_count < sim->cfg.number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}
