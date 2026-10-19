/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/19 12:00:00 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

# include "types.h"

int		init_dongles(t_sim *sim);
void	destroy_dongles(t_sim *sim);
void	init_request(t_request *request, t_coder *coder);
int		wait_tick(t_dongle *dongle);
int		wait_dongle(t_coder *coder, t_dongle *dongle, t_request *request);
int		wait_pair(t_coder *coder, t_request *request, t_dongle **set);
void	order_pair(t_coder *coder, t_dongle **set);
void	lock_dongles(t_dongle **set);
void	unlock_dongles(t_dongle **set);
int		acquire_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);

#endif
