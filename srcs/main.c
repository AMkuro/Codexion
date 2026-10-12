/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 21:00:42 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	main(int argc, char *argv[])
{
	t_config	cfg;
	t_sim		sim;
	int			status;

	status = parse_args(argc, argv, &cfg);
	if (status != EXIT_OK)
		return (status);
	if (init_simulation(&sim, &cfg) != EXIT_OK)
	{
		cleanup_simulation(&sim);
		fprintf(stderr, "Error: simulation initialization failed\n");
		return (EXIT_ERROR);
	}
	status = run_simulation(&sim);
	cleanup_simulation(&sim);
	return (status);
}
