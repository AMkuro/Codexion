/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static void	link_coder_dongles(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	count = sim->cfg.number_of_coders;
	while (i < count)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		if (count != 1)
			sim->coders[i].right = &sim->dongles[(i + 1) % count];
		i++;
	}
}

static int	allocate_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->cfg.number_of_coders);
	if (sim->coders == NULL)
		return (EXIT_ERROR);
	memset(sim->coders, 0, sizeof(t_coder) * sim->cfg.number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->cfg.number_of_coders);
	if (sim->dongles == NULL)
		return (EXIT_ERROR);
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->cfg.number_of_coders);
	return (EXIT_OK);
}

int	init_simulation(t_sim *sim, const t_config *cfg)
{
	memset(sim, 0, sizeof(*sim));
	sim->cfg = *cfg;
	if (allocate_arrays(sim) != EXIT_OK
		|| init_sim_mutexes(sim) != EXIT_OK
		|| init_dongles(sim) != EXIT_OK)
		return (EXIT_ERROR);
	link_coder_dongles(sim);
	return (EXIT_OK);
}

static int	start_coders(t_sim *sim, int *created)
{
	*created = 0;
	while (*created < sim->cfg.number_of_coders)
	{
		if (pthread_create(&sim->coders[*created].thread, NULL, coder_routine,
				&sim->coders[*created]) != 0)
			return (EXIT_ERROR);
		(*created)++;
	}
	return (EXIT_OK);
}

int	run_simulation(t_sim *sim)
{
	int	i;
	int	created;
	int	status;

	i = 0;
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
		return (EXIT_ERROR);
	status = start_coders(sim, &created);
	if (status != EXIT_OK)
		sim_stop(sim);
	else
		start_simulation(sim);
	while (i < created)
		pthread_join(sim->coders[i++].thread, NULL);
	sim_stop(sim);
	pthread_join(sim->monitor_thread, NULL);
	return (status);
}
