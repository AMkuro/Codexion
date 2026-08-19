#include "../../includes/codexion.h"

static void	swap_requests(t_request **left, t_request **right)
{
	t_request	*tmp;

	tmp = *left;
	*left = *right;
	*right = tmp;
}

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

int	heap_push(t_heap *heap, t_request *request)
{
	size_t	index;
	size_t	parent;

	if (heap->size >= heap->capacity)
		return (EXIT_ERROR);
	index = heap->size++;
	heap->items[index] = request;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (heap_request_before(heap, heap->items[parent], heap->items[index]))
			break ;
		swap_requests(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
	return (EXIT_OK);
}

t_request	*heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (heap->items[0]);
}

static void	heapify_down(t_heap *heap)
{
	size_t	index;
	size_t	left;
	size_t	right;
	size_t	best;

	index = 0;
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

t_request	*heap_pop(t_heap *heap)
{
	t_request	*top;

	if (heap->size == 0)
		return (NULL);
	top = heap->items[0];
	heap->items[0] = heap->items[--heap->size];
	heapify_down(heap);
	return (top);
}
