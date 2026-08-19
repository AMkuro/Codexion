#include "../../includes/codexion.h"

long	now_ms(void)
{
	struct timeval	 tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

long	sim_elapsed_ms(t_sim *sim)
{
	return (now_ms() - sim->start_ms);
}
