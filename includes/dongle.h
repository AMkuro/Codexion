#ifndef DONGLE_H
# define DONGLE_H

# include "types.h"

int		init_dongles(t_sim *sim);
void	destroy_dongles(t_sim *sim);
int		init_request(t_request *request, t_coder *coder);
void	destroy_request(t_request *request);
int		acquire_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);

#endif
