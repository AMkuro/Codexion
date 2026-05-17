/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 21:00:42 by miwasaki          #+#    #+#             */
/*   Updated: 2026/05/15 22:07:14 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	main(int argc, char *argv[])
{
	t_config	cfg;

	if (argc != 9)
	{
		fprintf(stderr, "Error: invalid number of arguments\n");
		fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout ");
		fprintf(stderr, "time_to_compile time_to_debug time_to_refactor ");
		fprintf(stderr, "number_of_compiles_required ");
		fprintf(stderr, "dongle_cooldown scheduler\n");
		return (64);
	}
	return (0);
}
