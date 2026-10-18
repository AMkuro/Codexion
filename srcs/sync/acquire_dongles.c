/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static int	acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	request;
	int			status;

	pthread_mutex_lock(&dongle->mutex);
	init_request(&request, coder, dongle);
	status = heap_push(&dongle->queue, &request);
	if (status == EXIT_OK)
		status = wait_dongle(coder, dongle, &request);
	else
		sim_stop(coder->sim);
	pthread_mutex_unlock(&dongle->mutex);
	if (status == EXIT_OK)
		log_state(coder->sim, coder->id, "has taken a dongle");
	return (status);
}

static int	dongle_rank(t_dongle *dongle, int count)
{
	if (count % 2 != 0 && dongle->id == count)
		return (2);
	return ((dongle->id - 1) % 2);
}

/* Ring coloring limits hold-and-wait chains to at most two edges. */
static void	order_dongles(t_dongle **first, t_dongle **second, int count)
{
	t_dongle	*tmp;

	if (*second != NULL && dongle_rank(*first, count)
		> dongle_rank(*second, count))
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
	order_dongles(&first, &second, coder->sim->cfg.number_of_coders);
	if (acquire_one_dongle(coder, first) != EXIT_OK)
		return (EXIT_ERROR);
	if (second == NULL)
	{
		while (!sim_should_stop(coder->sim))
			usleep(500);
	}
	else if (acquire_one_dongle(coder, second) == EXIT_OK)
		return (EXIT_OK);
	release_dongles(coder);
	return (EXIT_ERROR);
}
