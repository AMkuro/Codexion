#include "../../includes/codexion.h"

int	fifo_request_before(t_request *left, t_request *right)
{
	if (left->arrival_seq == right->arrival_seq)
		return (left->coder_id < right->coder_id);
	return (left->arrival_seq < right->arrival_seq);
}
