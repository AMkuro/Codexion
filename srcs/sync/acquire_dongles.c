#include "../../includes/codexion.h"

static int	acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
	if (dongle == NULL)
		return (EXIT_ERROR);
	pthread_mutex_lock(&dongle->mutex);
	while ((dongle->owner_id != 0 || now_ms() < dongle->available_at_ms)
		&& !sim_should_stop(coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		usleep(500);
		pthread_mutex_lock(&dongle->mutex);
	}
	if (sim_should_stop(coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (EXIT_ERROR);
	}
	dongle->owner_id = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	log_state(coder->sim, coder->id, "has taken a dongle");
	return (EXIT_OK);
}

static void	order_dongles(t_dongle **first, t_dongle **second)
{
	t_dongle	*tmp;

	if (*second != NULL && (*first)->id > (*second)->id)
	{
		tmp = *first;
		*first = *second;
		*second = tmp;
	}
}

int	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left;
	second = coder->right;
	if (second == NULL)
		return (EXIT_ERROR);
	order_dongles(&first, &second);
	if (acquire_one_dongle(coder, first) != EXIT_OK)
		return (EXIT_ERROR);
	if (acquire_one_dongle(coder, second) != EXIT_OK)
	{
		release_dongles(coder);
		return (EXIT_ERROR);
	}
	return (EXIT_OK);
}
