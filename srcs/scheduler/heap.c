/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	heap_init(t_heap *heap, size_t capacity, t_scheduler_kind scheduler)
{
	memset(heap, 0, sizeof(*heap));
	heap->items = malloc(sizeof(t_request *) * capacity);
	if (heap->items == NULL)
		return (EXIT_ERROR);
	heap->capacity = capacity;
	heap->scheduler = scheduler;
	return (EXIT_OK);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->items);
	memset(heap, 0, sizeof(*heap));
}

t_request	*heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (heap->items[0]);
}
