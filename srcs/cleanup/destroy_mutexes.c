/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_mutexes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	init_sim_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (EXIT_ERROR);
	sim->init_stage = 1;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (EXIT_ERROR);
	sim->init_stage = 2;
	if (pthread_cond_init(&sim->start_cond, NULL) != 0)
		return (EXIT_ERROR);
	sim->init_stage = 3;
	return (EXIT_OK);
}

void	destroy_mutexes(t_sim *sim)
{
	if (sim->init_stage >= 3)
		pthread_cond_destroy(&sim->start_cond);
	if (sim->init_stage >= 2)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->init_stage >= 1)
		pthread_mutex_destroy(&sim->stop_mutex);
}
