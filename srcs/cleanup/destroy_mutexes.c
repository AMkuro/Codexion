#include "../../includes/codexion.h"

int	init_coder_mutexes(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (EXIT_ERROR);
		i++;
	}
	return (EXIT_OK);
}

void	destroy_mutexes(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->coders != NULL && i < sim->cfg.number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}
