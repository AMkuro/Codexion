/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 21:45:52 by miwasaki          #+#    #+#             */
/*   Updated: 2026/05/17 21:45:57 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


	if (argc != 9)
	{
		fprintf(stderr, "Error: invalid number of arguments\n");
		fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout ");
		fprintf(stderr, "time_to_compile time_to_debug time_to_refactor ");
		fprintf(stderr, "number_of_compiles_required ");
		fprintf(stderr, "dongle_cooldown scheduler\n");
		return (64);
	}
