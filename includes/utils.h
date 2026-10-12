/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 18:32:08 by miwasaki          #+#    #+#             */
/*   Updated: 2026/10/11 18:32:08 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "types.h"

int		parse_args(int argc, char **argv, t_config *cfg);
int		parse_positive_int(const char *str, int *out);
int		parse_non_negative_long(const char *str, long *out);
int		validate_config(const t_config *cfg);
long	now_ms(void);
long	sim_elapsed_ms(t_sim *sim);
void	sleep_ms(t_sim *sim, long duration_ms);
void	log_state(t_sim *sim, int coder_id, const char *message);
int		begin_compile(t_coder *coder);
void	finish_compile(t_coder *coder);
int		init_simulation(t_sim *sim, const t_config *cfg);
int		run_simulation(t_sim *sim);
void	cleanup_simulation(t_sim *sim);
int		init_sim_mutexes(t_sim *sim);
void	destroy_mutexes(t_sim *sim);
void	*coder_routine(void *arg);
int		coder_compile(t_coder *coder);
void	coder_debug(t_coder *coder);
void	coder_refactor(t_coder *coder);
int		sim_should_stop(t_sim *sim);
void	sim_stop(t_sim *sim);
int		wait_for_start(t_sim *sim);
void	start_simulation(t_sim *sim);

#endif
