/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:01 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/24 16:53:42 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	init_data(t_data *data, int argc, char **argv)
{
	data->philos_nb = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->num_of_meals = ft_atoi(argv[5]);
	else
		data->num_of_meals = -1;
	data->simulation_end = false;
	if (init_mutexes(data) != 0)
		return (1);
	if (init_forks(data) != 0)
		return (1);
	if (init_philosophers(data) != 0)
		return (1);
	return (0);
}

int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->end_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->meal_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->data_mutex, NULL) != 0)
		return (1);
	return (0);
}

int	init_forks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(t_fork) * data->philos_nb);
	if (!data->forks)
		return (1);
	i = 0;
	while (i < data->philos_nb)
	{
		data->forks[i].id = i;
		if (pthread_mutex_init(&data->forks[i].mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	init_philosophers(t_data *data)
{
	int	i;

	data->philos = malloc(sizeof(t_philo) * data->philos_nb);
	if (!data->philos)
		return (1);
	memset(data->philos, 0, sizeof(t_philo) * data->philos_nb);
	i = 0;
	while (i < data->philos_nb)
	{
		data->philos[i].id = i + 1;
		data->philos[i].meals_eaten = 0;
		data->philos[i].last_meal_time = 0;
		data->philos[i].data = data;
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philos_nb];
		i++;
	}
	return (0);
}
