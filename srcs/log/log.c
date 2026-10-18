/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *message)
{
	long	now;

	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	now = now_ms();
	if (!check_burnout_locked(sim, now))
		printf("%ld %d %s\n", now - sim->start_ms, coder_id, message);
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->log_mutex);
}

int	begin_compile(t_coder *coder)
{
	t_sim	*sim;
	long	now;
	int		status;

	sim = coder->sim;
	status = EXIT_ERROR;
	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	now = now_ms();
	if (!check_burnout_locked(sim, now))
	{
		coder->last_compile_start_ms = now;
		printf("%ld %d is compiling\n", now - sim->start_ms, coder->id);
		status = EXIT_OK;
	}
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->log_mutex);
	return (status);
}

void	finish_compile(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	if (!check_burnout_locked(sim, now_ms()))
	{
		if (coder->compile_count < sim->cfg.number_of_compiles_required)
			coder->compile_count++;
		if (all_coders_completed(sim))
			sim->stop = 1;
	}
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->log_mutex);
}
