#include "../../includes/codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->right == NULL)
	{
		while (!sim_should_stop(coder->sim))
			usleep(500);
		return (NULL);
	}
	while (!sim_should_stop(coder->sim)
		&& coder->compile_count < coder->sim->cfg.number_of_compiles_required)
	{
		coder_compile(coder);
		coder_debug(coder);
		coder_refactor(coder);
	}
	return (NULL);
}
