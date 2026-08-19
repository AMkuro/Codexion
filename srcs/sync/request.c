#include "../../includes/codexion.h"

int	init_request(t_request *request, t_coder *coder)
{
	memset(request, 0, sizeof(*request));
	request->coder_id = coder->id;
	request->arrival_seq = now_ms();
	request->deadline_ms = coder->last_compile_start_ms
		+ coder->sim->cfg.time_to_burnout;
	if (pthread_cond_init(&request->cond, NULL) != 0)
		return (EXIT_ERROR);
	return (EXIT_OK);
}

void	destroy_request(t_request *request)
{
	pthread_cond_destroy(&request->cond);
}
