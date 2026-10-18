/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static int	init_one_dongle(t_dongle *dongle, t_scheduler_kind scheduler)
{
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (EXIT_ERROR);
	dongle->init_stage = 1;
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
		return (EXIT_ERROR);
	dongle->init_stage = 2;
	if (heap_init(&dongle->queue, 2, scheduler) != EXIT_OK)
		return (EXIT_ERROR);
	dongle->init_stage = 3;
	return (EXIT_OK);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->dongles[i].id = i + 1;
		if (init_one_dongle(&sim->dongles[i], sim->cfg.scheduler) != EXIT_OK)
			return (EXIT_ERROR);
		i++;
	}
	return (EXIT_OK);
}

void	destroy_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->dongles != NULL && i < sim->cfg.number_of_coders)
	{
		if (sim->dongles[i].init_stage >= 3)
			heap_destroy(&sim->dongles[i].queue);
		if (sim->dongles[i].init_stage >= 2)
			pthread_cond_destroy(&sim->dongles[i].cond);
		if (sim->dongles[i].init_stage >= 1)
			pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
}
