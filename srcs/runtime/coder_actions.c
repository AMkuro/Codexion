#include "../../includes/codexion.h"

void	coder_compile(t_coder *coder)
{
	if (acquire_dongles(coder) != EXIT_OK)
	{
		sleep_ms(coder->sim, 1);
		return ;
	}
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start_ms = now_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	log_state(coder->sim, coder->id, "is compiling");
	sleep_ms(coder->sim, coder->sim->cfg.time_to_compile);
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mutex);
	release_dongles(coder);
}

void	coder_debug(t_coder *coder)
{
	if (sim_should_stop(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is debugging");
	sleep_ms(coder->sim, coder->sim->cfg.time_to_debug);
}

void	coder_refactor(t_coder *coder)
{
	if (sim_should_stop(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is refactoring");
	sleep_ms(coder->sim, coder->sim->cfg.time_to_refactor);
}
