/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 18:24:44 by miwasaki          #+#    #+#             */
/*   Updated: 2026/06/03 21:37:14 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "types.h"

int		parse_args(int argc, char **argv, t_config *cfg);
int		parse_positive_int(const char *str, unsigned int *out);
int		parse_non_negative_long(const char *str, unsigned long *out);
int		validate_config(const t_config *cfg);
long	now_ms(void);
long	sim_elapsed_ms(t_sim *sim);
void	sleep_ms(t_sim *sim, long duration_ms);
void	log_state(t_sim *sim, int coder_id, const char *message);
void	log_burnout(t_sim *sim, int coder_id);

int		init_simulation(t_sim *sim, const t_config *cfg);
int		run_simulation(t_sim *sim);
void	cleanup_simulation(t_sim *sim);
void	destroy_mutexes(t_sim *sim);
int		init_coder_mutexes(t_sim *sim);
void	*monitor_routine(void *arg);
int		all_coders_completed(t_sim *sim);
void	*coder_routine(void *arg);
void	coder_compile(t_coder *coder);
void	coder_debug(t_coder *coder);
void	coder_refactor(t_coder *coder);
int		sim_should_stop(t_sim *sim);
void	sim_stop(t_sim *sim);

#endif
