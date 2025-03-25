/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:15:43 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/25 17:04:30 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	init_meal_times(t_data *data)
{
	int	i;
	int	num_philos;

	pthread_mutex_lock(&data->data_mutex);
	num_philos = data->philos_nb;
	pthread_mutex_unlock(&data->data_mutex);
	i = 0;
	while (i < num_philos)
	{
		pthread_mutex_lock(&data->meal_mutex);
		data->philos[i].last_meal_time = time_since_start(data);
		pthread_mutex_unlock(&data->meal_mutex);
		i++;
	}
}

static bool	check_meal_limit(t_philo *philo)
{
	bool	should_exit;
	int		num_meals;

	pthread_mutex_lock(&philo->data->data_mutex);
	num_meals = philo->data->num_of_meals;
	pthread_mutex_unlock(&philo->data->data_mutex);
	if (num_meals == -1)
		return (false);
	pthread_mutex_lock(&philo->data->meal_mutex);
	should_exit = (philo->meals_eaten >= num_meals);
	pthread_mutex_unlock(&philo->data->meal_mutex);
	return (should_exit);
}

bool	try_acquire_forks(t_philo *philo)
{
	unsigned int	attempts;
	bool			is_urgent;

	attempts = 0;
	update_urgency(philo, &is_urgent);
	while (!take_forks(philo) && !get_simulation_end(philo->data))
	{
		attempts++;
		handle_backoff(philo, is_urgent);
		if (attempts % 5 == 0)
			update_urgency(philo, &is_urgent);
	}
	return (!get_simulation_end(philo->data));
}

void	stagger_start(t_philo *philo)
{
	int		num_philos;
	long	time_to_eat;

	pthread_mutex_lock(&philo->data->data_mutex);
	num_philos = philo->data->philos_nb;
	time_to_eat = philo->data->time_to_eat;
	pthread_mutex_unlock(&philo->data->data_mutex);
	if (num_philos > 100)
	{
		if (philo->id % 2 == 0)
			custom_sleep(time_to_eat / 2, philo->data);
		else
			custom_sleep((philo->id % 10) * 5, philo->data);
	}
	else if (philo->id % 2 == 0)
		custom_sleep(10, philo->data);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	stagger_start(philo);
	while (!get_simulation_end(philo->data))
	{
		if (!try_acquire_forks(philo))
			break ;
		eat(philo);
		if (check_meal_limit(philo))
			break ;
		handle_post_eating(philo);
	}
	return (NULL);
}
