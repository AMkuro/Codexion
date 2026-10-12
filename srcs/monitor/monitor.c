/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* The caller holds log_mutex, then stop_mutex. */
int	check_burnout_locked(t_sim *sim, long now)
{
	int	i;
	int	first;

	if (sim->stop || !sim->started)
		return (sim->stop);
	i = 0;
	first = 0;
	while (++i < sim->cfg.number_of_coders)
	{
		if (sim->coders[i].last_compile_start_ms
			< sim->coders[first].last_compile_start_ms)
			first = i;
	}
	if (now - sim->coders[first].last_compile_start_ms
		>= sim->cfg.time_to_burnout)
	{
		sim->stop = 1;
		printf("%ld %d burned out\n", now - sim->start_ms,
			sim->coders[first].id);
	}
	return (sim->stop);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	if (wait_for_start(sim) != EXIT_OK)
		return (NULL);
	while (!sim_should_stop(sim))
	{
		pthread_mutex_lock(&sim->log_mutex);
		pthread_mutex_lock(&sim->stop_mutex);
		check_burnout_locked(sim, now_ms());
		pthread_mutex_unlock(&sim->stop_mutex);
		pthread_mutex_unlock(&sim->log_mutex);
		usleep(500);
	}
	return (NULL);
}
