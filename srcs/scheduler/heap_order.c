/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_order.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static void	swap_requests(t_request **left, t_request **right)
{
	t_request	*tmp;

	tmp = *left;
	*left = *right;
	*right = tmp;
}

size_t	heap_sift_up(t_heap *heap, size_t index)
{
	size_t	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!heap_request_before(heap, heap->items[index], heap->items[parent]))
			break ;
		swap_requests(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
	return (index);
}

void	heap_sift_down(t_heap *heap, size_t index)
{
	size_t	left;
	size_t	right;
	size_t	best;

	while (index < heap->size)
	{
		left = (index * 2) + 1;
		right = left + 1;
		best = index;
		if (left < heap->size
			&& heap_request_before(heap, heap->items[left], heap->items[best]))
			best = left;
		if (right < heap->size
			&& heap_request_before(heap, heap->items[right], heap->items[best]))
			best = right;
		if (best == index)
			break ;
		swap_requests(&heap->items[index], &heap->items[best]);
		index = best;
	}
}
