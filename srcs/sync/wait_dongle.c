/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/12 22:52:37 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"
#include <errno.h>
#include <time.h>

int	wait_tick(t_dongle *dongle)
{
	struct timeval	tv;
	struct timespec	until;
	int				status;

	gettimeofday(&tv, NULL);
	until.tv_sec = tv.tv_sec;
	until.tv_nsec = (tv.tv_usec * 1000L) + 1000000L;
	if (until.tv_nsec >= 1000000000L)
	{
		until.tv_sec++;
		until.tv_nsec -= 1000000000L;
	}
	status = pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &until);
	return (status != 0 && status != ETIMEDOUT);
}

int	wait_dongle(t_coder *coder, t_dongle *dongle, t_request *request)
{
	while (!sim_should_stop(coder->sim))
	{
		if (heap_peek(&dongle->queue) == request
			&& dongle->owner_id == 0
			&& now_ms() >= dongle->available_at_ms)
		{
			heap_pop(&dongle->queue);
			dongle->owner_id = coder->id;
			return (EXIT_OK);
		}
		if (wait_tick(dongle) != 0)
		{
			sim_stop(coder->sim);
			break ;
		}
	}
	heap_remove(&dongle->queue, request);
	pthread_cond_broadcast(&dongle->cond);
	return (EXIT_ERROR);
}
