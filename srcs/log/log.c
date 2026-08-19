#include "../../includes/codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *message)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_should_stop(sim))
		printf("%ld %d %s\n", sim_elapsed_ms(sim), coder_id, message);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	log_burnout(t_sim *sim, int coder_id)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d burned out\n", sim_elapsed_ms(sim), coder_id);
	pthread_mutex_unlock(&sim->log_mutex);
}
