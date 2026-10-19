/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_pair.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 12:00:00 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* Each dongle serves only its heap head, so the earliest deadline wins. */
static int	is_ready(t_dongle *dongle, t_request *request, long now)
{
	return (heap_peek(&dongle->queue) == request && dongle->owner_id == 0
		&& now >= dongle->available_at_ms);
}

static int	take_pair(t_coder *coder, t_request *request)
{
	long	now;

	now = now_ms();
	if (!is_ready(coder->left, request, now)
		|| !is_ready(coder->right, request, now))
		return (0);
	heap_remove(&coder->left->queue, request);
	heap_remove(&coder->right->queue, request);
	coder->left->owner_id = coder->id;
	coder->right->owner_id = coder->id;
	return (1);
}

int	wait_pair(t_coder *coder, t_request *request, t_dongle **set)
{
	int	taken;
	int	failed;

	taken = 0;
	failed = 0;
	while (!taken && !failed && !sim_should_stop(coder->sim))
	{
		lock_dongles(set);
		taken = take_pair(coder, request);
		unlock_dongles(set);
		if (taken)
			break ;
		pthread_mutex_lock(&coder->left->mutex);
		failed = wait_tick(coder->left);
		pthread_mutex_unlock(&coder->left->mutex);
	}
	if (failed)
		sim_stop(coder->sim);
	if (taken)
		return (EXIT_OK);
	return (EXIT_ERROR);
}
