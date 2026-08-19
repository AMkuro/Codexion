/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_int.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:47:53 by miwasaki          #+#    #+#             */
/*   Updated: 2026/06/03 21:35:54 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static int	parse_digits(const char *str, unsigned long *value)
{
	size_t			i;
	unsigned long	next;

	i = 0;
	*value = 0;
	if (str[0] == '\0')
		return (EXIT_ERROR);
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (EXIT_ERROR);
		next = (*value * 10) + (str[i] - '0');
		if (next < *value)
			return (EXIT_ERROR);
		*value = next;
		i++;
	}
	return (EXIT_OK);
}

int	parse_positive_int(const char *str, int *out)
{
	unsigned long	value;

	if (parse_digits(str, &value) != EXIT_OK || value <= 0 || value > UINT_MAX)
	{
		fprintf(stderr, "Error: expected a positive integer\n");
		return (EXIT_USAGE);
	}
	*out = (int)value;
	return (EXIT_OK);
}

int	parse_non_negative_long(const char *str, long *out)
{
	unsigned long	value;

	if (parse_digits(str, &value) != EXIT_OK)
	{
		fprintf(stderr, "Error: expected a non-negative integer\n");
		return (EXIT_USAGE);
	}
	*out = value;
	return (EXIT_OK);
}
