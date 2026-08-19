#include "../../includes/codexion.h"

static int	coder_burned_out(t_sim *sim, t_coder *coder)
{
	long	last_compile;
	int		compile_count;

	pthread_mutex_lock(&coder->state_mutex);
	last_compile = coder->last_compile_start_ms;
	compile_count = coder->compile_count;
	pthread_mutex_unlock(&coder->state_mutex);
	if (compile_count >= sim->cfg.number_of_compiles_required)
		return (0);
	return (now_ms() - last_compile >= sim->cfg.time_to_burnout);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		if (all_coders_completed(sim))
		{
			sim_stop(sim);
			return (NULL);
		}
		i = 0;
		while (i < sim->cfg.number_of_coders)
		{
			if (coder_burned_out(sim, &sim->coders[i]))
			{
				log_burnout(sim, sim->coders[i].id);
				sim_stop(sim);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
