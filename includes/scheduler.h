/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "types.h"

int			heap_init(t_heap *heap, size_t capacity,
				t_scheduler_kind scheduler);
void		heap_destroy(t_heap *heap);
int			heap_push(t_heap *heap, t_request *request);
t_request	*heap_pop(t_heap *heap);
t_request	*heap_peek(t_heap *heap);
void		heap_remove(t_heap *heap, t_request *request);
size_t		heap_sift_up(t_heap *heap, size_t index);
void		heap_sift_down(t_heap *heap, size_t index);
int			heap_request_before(t_heap *heap, t_request *left,
				t_request *right);
int			fifo_request_before(t_request *left, t_request *right);
int			edf_request_before(t_request *left, t_request *right);

#endif
