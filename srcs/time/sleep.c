#include "../../includes/codexion.h"

void	sleep_ms(t_sim *sim, long duration_ms)
{
	long	start;

	start = now_ms();
	while (!sim_should_stop(sim) && now_ms() - start < duration_ms)
		usleep(500);
}
