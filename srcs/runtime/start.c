/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	wait_for_start(t_sim *sim)
{
	int	status;

	pthread_mutex_lock(&sim->stop_mutex);
	while (!sim->started && !sim->stop)
	{
		if (pthread_cond_wait(&sim->start_cond, &sim->stop_mutex) != 0)
		{
			sim->stop = 1;
			pthread_cond_broadcast(&sim->start_cond);
		}
	}
	status = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (status);
}

void	start_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->start_ms = now_ms();
	i = 0;
	while (i < sim->cfg.number_of_coders)
		sim->coders[i++].last_compile_start_ms = sim->start_ms;
	sim->started = 1;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->stop_mutex);
}
