/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_scheduler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* Ties: fewer compiles, then ring rank, then arrival order. */
int	edf_request_before(t_request *left, t_request *right)
{
	if (left->deadline_ms != right->deadline_ms)
		return (left->deadline_ms < right->deadline_ms);
	if (left->compile_count != right->compile_count)
		return (left->compile_count < right->compile_count);
	if (left->rank != right->rank)
		return (left->rank < right->rank);
	return (fifo_request_before(left, right));
}
