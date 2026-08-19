#include "../../includes/codexion.h"

static void	release_one_dongle(t_coder *coder, t_dongle *dongle)
{
	if (dongle == NULL)
		return ;
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner_id == coder->id)
	{
		dongle->owner_id = 0;
		dongle->available_at_ms = now_ms() + coder->sim->cfg.dongle_cooldown;
		pthread_cond_broadcast(&dongle->cond);
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongles(t_coder *coder)
{
	release_one_dongle(coder, coder->left);
	release_one_dongle(coder, coder->right);
}
