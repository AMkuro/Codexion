#include "../../includes/codexion.h"

int	edf_request_before(t_request *left, t_request *right)
{
	if (left->deadline_ms == right->deadline_ms)
		return (fifo_request_before(left, right));
	return (left->deadline_ms < right->deadline_ms);
}
