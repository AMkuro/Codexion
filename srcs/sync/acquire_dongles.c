/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/* A lone coder can take its only dongle but can never compile. */
static int	acquire_single(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->mutex);
	coder->left->owner_id = coder->id;
	pthread_mutex_unlock(&coder->left->mutex);
	log_state(coder->sim, coder->id, "has taken a dongle");
	while (!sim_should_stop(coder->sim))
		usleep(500);
	release_dongles(coder);
	return (EXIT_ERROR);
}

static int	push_pair(t_coder *coder, t_request *request)
{
	if (heap_push(&coder->left->queue, request) == EXIT_OK
		&& heap_push(&coder->right->queue, request) == EXIT_OK)
		return (EXIT_OK);
	sim_stop(coder->sim);
	return (EXIT_ERROR);
}

static void	cancel_pair(t_coder *coder, t_request *request)
{
	heap_remove(&coder->left->queue, request);
	heap_remove(&coder->right->queue, request);
}

/* Both dongles are granted together: no coder holds one while waiting. */
int	acquire_dongles(t_coder *coder)
{
	t_request	request;
	t_dongle	*set[2];
	int			status;

	if (coder->right == NULL)
		return (acquire_single(coder));
	order_pair(coder, set);
	lock_dongles(set);
	init_request(&request, coder);
	status = push_pair(coder, &request);
	unlock_dongles(set);
	if (status == EXIT_OK)
		status = wait_pair(coder, &request, set);
	if (status == EXIT_OK)
	{
		log_state(coder->sim, coder->id, "has taken a dongle");
		log_state(coder->sim, coder->id, "has taken a dongle");
		return (EXIT_OK);
	}
	lock_dongles(set);
	cancel_pair(coder, &request);
	unlock_dongles(set);
	return (EXIT_ERROR);
}
