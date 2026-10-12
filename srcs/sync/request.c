/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* The caller holds the dongle mutex, which orders request arrivals. */
void	init_request(t_request *request, t_coder *coder, t_dongle *dongle)
{
	request->coder_id = coder->id;
	request->arrival_seq = dongle->next_arrival++;
	pthread_mutex_lock(&coder->sim->stop_mutex);
	request->deadline_ms = coder->last_compile_start_ms
		+ coder->sim->cfg.time_to_burnout;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
}
