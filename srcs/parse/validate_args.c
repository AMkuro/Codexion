/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* Prevent allocation-size overflow on 32-bit systems before multiplication. */
int	validate_config(const t_config *cfg)
{
	if (cfg->time_to_burnout <= 0 || cfg->time_to_compile <= 0
		|| cfg->time_to_debug <= 0 || cfg->time_to_refactor <= 0)
	{
		fprintf(stderr, "Error: times except cooldown must be positive\n");
		return (EXIT_USAGE);
	}
	if ((size_t)cfg->number_of_coders > (size_t)-1 / sizeof(t_coder)
		|| (size_t)cfg->number_of_coders > (size_t)-1 / sizeof(t_dongle))
		return (EXIT_USAGE);
	return (EXIT_OK);
}
