#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "types.h"

int			heap_init(t_heap *heap, size_t capacity, t_scheduler_kind scheduler);
void		heap_destroy(t_heap *heap);
int			heap_push(t_heap *heap, t_request *request);
t_request	*heap_pop(t_heap *heap);
t_request	*heap_peek(t_heap *heap);
int			heap_request_before(t_heap *heap, t_request *left, t_request *right);
int			fifo_request_before(t_request *left, t_request *right);
int			edf_request_before(t_request *left, t_request *right);

#endif
