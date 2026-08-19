#include "../../includes/codexion.h"

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->dongles[i].id = i + 1;
		sim->dongles[i].owner_id = 0;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0
			|| pthread_cond_init(&sim->dongles[i].cond, NULL) != 0
			|| heap_init(&sim->dongles[i].queue,
				(size_t)sim->cfg.number_of_coders,
				sim->cfg.scheduler) != EXIT_OK)
			return (EXIT_ERROR);
		i++;
	}
	return (EXIT_OK);
}

void	destroy_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->dongles != NULL && i < sim->cfg.number_of_coders)
	{
		heap_destroy(&sim->dongles[i].queue);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
}
