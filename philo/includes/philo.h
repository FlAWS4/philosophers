/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:52 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/17 21:18:32 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>
# include <limits.h>
# include <stdbool.h>

# define EATING "is eating"
# define SLEEPING "is sleeping"
# define THINKING "is thinking"
# define FORK "has taken a fork"
# define DEAD "died"

typedef struct s_fork
{
	int				id;
	pthread_mutex_t	mutex;
}	t_fork;


typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long long		last_meal_time;
	pthread_t		thread;
	t_fork			*left_fork;
	t_fork			*right_fork;
	struct s_data	*data;
}					t_philo;

typedef struct s_data
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_of_meals;
	long long		start_time;
	bool			simulation_end;
	t_philo			*philosophers;
	t_fork			*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	end_mutex;
	pthread_mutex_t	meal_mutexes;
}					t_data;

// init.c
int			init_data(t_data *data, int argc, char **argv);
int			init_mutexes(t_data *data);
int			init_philosophers(t_data *data);
int			init_forks(t_data *data);

// philosophers.c
void		*philosopher_routine(void *arg);
int			start_simulation(t_data *data);
void		join_philosophers(t_data *data);

// actions.c
void		take_forks(t_philo *philo);
void		eat(t_philo *philo);
void		sleep_think(t_philo *philo);

// monitor.c
void		*monitor_routine(void *arg);
bool		check_if_simulation_should_end(t_data *data);
bool		get_simulation_end(t_data *data);
void		set_simulation_end(t_data *data);

// utils.c
int			ft_atoi(const char *str);
int			error_exit(char *msg);
void		print_status(t_philo *philo, char *status);
bool		parse_args(int argc, char **argv, t_data *data);

// time.c
long long	get_time_in_ms(void);
void		custom_sleep(long long time);
long long	time_since_start(t_data *data);
void		update_last_meal(t_philo *philo);
long long	get_last_meal_time(t_philo *philo);

#endif