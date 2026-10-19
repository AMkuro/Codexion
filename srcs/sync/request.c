/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* Adjacent coders get different ranks, so equal-deadline ties spread out. */
static int	ring_rank(t_coder *coder)
{
	int	count;

	count = coder->sim->cfg.number_of_coders;
	if (count % 2 != 0 && coder->id == count)
		return (2);
	return ((coder->id - 1) % 2);
}

/* The caller holds the requested dongles' mutexes, which order arrivals. */
void	init_request(t_request *request, t_coder *coder)
{
	request->coder_id = coder->id;
	pthread_mutex_lock(&coder->sim->stop_mutex);
	request->arrival_seq = coder->sim->next_arrival++;
	request->deadline_ms = coder->last_compile_start_ms
		+ coder->sim->cfg.time_to_burnout;
	request->compile_count = coder->compile_count;
	request->rank = ring_rank(coder);
	pthread_mutex_unlock(&coder->sim->stop_mutex);
}
