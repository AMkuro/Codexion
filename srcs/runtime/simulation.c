/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 21:14:42 by miwasaki          #+#    #+#             */
/*   Updated: 2026/05/23 21:14:45 by miwasaki         ###   ########.fr       */
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
		sim->coders[i].last_compile_start_ms = sim->start_ms;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		if (count == 1)
			sim->coders[i].right = NULL;
		else
			sim->coders[i].right = &sim->dongles[(i + 1) % count];
		i++;
	}
}

int	init_simulation(t_sim *sim, const t_config *cfg)
{
	memset(sim, 0, sizeof(*sim));
	sim->cfg = *cfg;
	sim->coders = malloc(sizeof(t_coder) * cfg->number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * cfg->number_of_coders);
	if (sim->coders == NULL || sim->dongles == NULL)
		return (EXIT_ERROR);
	memset(sim->coders, 0, sizeof(t_coder) * cfg->number_of_coders);
	memset(sim->dongles, 0, sizeof(t_dongle) * cfg->number_of_coders);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0
		|| pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (EXIT_ERROR);
	if (init_dongles(sim) != EXIT_OK)
		return (EXIT_ERROR);
	sim->start_ms = now_ms();
	link_coder_dongles(sim);
	if (init_coder_mutexes(sim) != EXIT_OK)
		return (EXIT_ERROR);
	return (EXIT_OK);
}

static int	start_coders(t_sim *sim, int *created)
{
	int	i;

	i = 0;
	*created = 0;
	while (i < sim->cfg.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			return (EXIT_ERROR);
		(*created)++;
		i++;
	}
	return (EXIT_OK);
}

int	run_simulation(t_sim *sim)
{
	int	i;
	int	created;
	int	status;

	i = 0;
	created = 0;
	status = EXIT_OK;
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
		return (EXIT_ERROR);
	if (start_coders(sim, &created) != EXIT_OK)
	{
		sim_stop(sim);
		status = EXIT_ERROR;
	}
	while (i < created)
		pthread_join(sim->coders[i++].thread, NULL);
	sim_stop(sim);
	pthread_join(sim->monitor_thread, NULL);
	return (status);
}
