/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 21:45:52 by miwasaki          #+#    #+#             */
/*   Updated: 2026/06/03 17:50:19 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static void	print_argcount(void)
{
	fprintf(stderr, "Error: invalid number of arguments\n");
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout ");
	fprintf(stderr, "time_to_compile time_to_debug time_to_refactor ");
	fprintf(stderr, "number_of_compiles_required ");
	fprintf(stderr, "dongle_cooldown scheduler\n");
}

static int	parse_scheduler(const char *value, t_scheduler_kind *scheduler)
{
	if (strcmp(value, SCHEDULER_FIFO) == 0)
	{
		*scheduler = SCHED_KIND_FIFO;
		return (EXIT_OK);
	}
	if (strcmp(value, SCHEDULER_EDF) == 0)
	{
		*scheduler = SCHED_KIND_EDF;
		return (EXIT_OK);
	}
	fprintf(stderr, "Error: scheduler must be fifo or edf\n");
	return (EXIT_USAGE);
}

int	parse_args(int argc, char **argv, t_config *cfg)
{
	memset(cfg, 0, sizeof(*cfg));
	if (argc != CODEXION_ARGC)
	{
		print_argcount();
		return (EXIT_USAGE);
	}
	if (parse_positive_int(argv[1], &cfg->number_of_coders) != EXIT_OK
		|| parse_non_negative_long(argv[2], &cfg->time_to_burnout) != EXIT_OK
		|| parse_non_negative_long(argv[3], &cfg->time_to_compile) != EXIT_OK
		|| parse_non_negative_long(argv[4], &cfg->time_to_debug) != EXIT_OK
		|| parse_non_negative_long(argv[5], &cfg->time_to_refactor) != EXIT_OK
		|| parse_positive_int(argv[6],
			&cfg->number_of_compiles_required) != EXIT_OK
		|| parse_non_negative_long(argv[7], &cfg->dongle_cooldown) != EXIT_OK
		|| parse_scheduler(argv[8], &cfg->scheduler) != EXIT_OK)
		return (EXIT_USAGE);
	return (validate_config(cfg));
}
