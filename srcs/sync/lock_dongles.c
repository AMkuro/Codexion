/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 12:00:00 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* Every coder locks its pair in ascending ID order, ruling out lock cycles. */
void	order_pair(t_coder *coder, t_dongle **set)
{
	set[0] = coder->left;
	set[1] = coder->right;
	if (set[0]->id > set[1]->id)
	{
		set[0] = coder->right;
		set[1] = coder->left;
	}
}

void	lock_dongles(t_dongle **set)
{
	pthread_mutex_lock(&set[0]->mutex);
	pthread_mutex_lock(&set[1]->mutex);
}

void	unlock_dongles(t_dongle **set)
{
	pthread_mutex_unlock(&set[1]->mutex);
	pthread_mutex_unlock(&set[0]->mutex);
}
