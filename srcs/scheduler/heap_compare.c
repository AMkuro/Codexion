#include "../../includes/codexion.h"

int	heap_request_before(t_heap *heap, t_request *left, t_request *right)
{
	if (heap->scheduler == SCHED_KIND_EDF)
		return (edf_request_before(left, right));
	return (fifo_request_before(left, right));
}
