/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miwasaki <miwasaki@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 18:04:34 by miwasaki          #+#    #+#             */
/*   Updated: 2026/06/03 18:56:38 by miwasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include <pthread.h>
# include <stddef.h>

typedef enum e_scheduler_kind
{
	SCHED_KIND_FIFO,
	SCHED_KIND_EDF
}						t_scheduler_kind;

typedef struct s_config
{
	unsigned int		number_of_coders;
	unsigned long		time_to_burnout;
	unsigned long		time_to_compile;
	unsigned long		time_to_debug;
	unsigned long		time_to_refactor;
	unsigned int		number_of_compiles_required;
	unsigned long		dongle_cooldown;
	t_scheduler_kind	scheduler;
}						t_config;

typedef struct s_request
{
	unsigned int		coder_id;
	long				arrival_seq;
	long				deadline_ms;
	int					granted;
	pthread_cond_t		cond;
}						t_request;

typedef struct s_heap
{
	t_request			**items;
	size_t				size;
	size_t				capacity;
	t_scheduler_kind	scheduler;
}						t_heap;

typedef struct s_dongle
{
	int					id;
	int					owner_id;
	long				available_at_ms;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	t_heap				queue;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	int					compile_count;
	long				last_compile_start_ms;
	pthread_mutex_t		state_mutex;
	pthread_t			thread;
	t_dongle			*left;
	t_dongle			*right;
	struct s_sim		*sim;
}						t_coder;

typedef struct s_sim
{
	t_config			cfg;
	long				start_ms;
	int					stop;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		log_mutex;
	pthread_t			monitor_thread;
	t_coder				*coders;
	t_dongle			*dongles;
}						t_sim;

#endif
