/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	heap_push(t_heap *heap, t_request *request)
{
	size_t	index;

	if (heap->size >= heap->capacity)
		return (EXIT_ERROR);
	index = heap->size++;
	heap->items[index] = request;
	heap_sift_up(heap, index);
	return (EXIT_OK);
}

t_request	*heap_pop(t_heap *heap)
{
	t_request	*request;

	request = heap_peek(heap);
	if (request == NULL)
		return (NULL);
	heap_remove(heap, request);
	return (request);
}

void	heap_remove(t_heap *heap, t_request *request)
{
	size_t	index;

	index = 0;
	while (index < heap->size && heap->items[index] != request)
		index++;
	if (index == heap->size)
		return ;
	heap->size--;
	if (index == heap->size)
		return ;
	heap->items[index] = heap->items[heap->size];
	index = heap_sift_up(heap, index);
	heap_sift_down(heap, index);
}
