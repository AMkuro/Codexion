/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	coder_compile(t_coder *coder)
{
	if (acquire_dongles(coder) != EXIT_OK)
		return (EXIT_ERROR);
	if (begin_compile(coder) != EXIT_OK)
	{
		release_dongles(coder);
		return (EXIT_ERROR);
	}
	sleep_ms(coder->sim, coder->sim->cfg.time_to_compile);
	finish_compile(coder);
	release_dongles(coder);
	return (EXIT_OK);
}

void	coder_debug(t_coder *coder)
{
	if (sim_should_stop(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is debugging");
	sleep_ms(coder->sim, coder->sim->cfg.time_to_debug);
}

void	coder_refactor(t_coder *coder)
{
	if (sim_should_stop(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is refactoring");
	sleep_ms(coder->sim, coder->sim->cfg.time_to_refactor);
}
