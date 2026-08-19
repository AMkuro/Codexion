#include "../../includes/codexion.h"

int	validate_config(const t_config *cfg)
{
	if (cfg->time_to_burnout <= 0 || cfg->time_to_compile <= 0
		|| cfg->time_to_debug <= 0 || cfg->time_to_refactor <= 0)
	{
		fprintf(stderr, "Error: time values except cooldown must be positive\n");
		return (EXIT_USAGE);
	}
	return (EXIT_OK);
}
