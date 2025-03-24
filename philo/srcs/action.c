/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:11:58 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/24 16:52:57 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static bool	handle_single_philo(t_philo *philo)
{
	while (!get_simulation_end(philo->data))
		usleep(1000);
	pthread_mutex_unlock(&philo->left_fork->mutex);
	return (false);
}

static bool	take_forks_in_order(t_philo *philo, t_fork *first, t_fork *second)
{
	bool	is_single_philo;

	pthread_mutex_lock(&philo->data->data_mutex);
	is_single_philo = (philo->data->philos_nb == 1);
	pthread_mutex_unlock(&philo->data->data_mutex);
	pthread_mutex_lock(&first->mutex);
	print_status(philo, FORK);
	if (get_simulation_end(philo->data))
	{
		pthread_mutex_unlock(&first->mutex);
		return (false);
	}
	if (is_single_philo)
		return (handle_single_philo(philo));
	pthread_mutex_lock(&second->mutex);
	print_status(philo, FORK);
	if (get_simulation_end(philo->data))
	{
		pthread_mutex_unlock(&first->mutex);
		pthread_mutex_unlock(&second->mutex);
		return (false);
	}
	return (true);
}

bool	take_forks(t_philo *philo)
{
	t_fork	*first_fork;
	t_fork	*second_fork;

	if (get_simulation_end(philo->data))
		return (false);
	if (philo->left_fork->id < philo->right_fork->id)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	return (take_forks_in_order(philo, first_fork, second_fork));
}

void	eat(t_philo *philo)
{
	long	time_to_eat;

	pthread_mutex_lock(&philo->data->data_mutex);
	time_to_eat = philo->data->time_to_eat;
	pthread_mutex_unlock(&philo->data->data_mutex);
	update_last_meal(philo);
	print_status(philo, EATING);
	custom_sleep(time_to_eat);
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	pthread_mutex_unlock(&philo->left_fork->mutex);
	pthread_mutex_unlock(&philo->right_fork->mutex);
}

void	sleep_think(t_philo *philo)
{
	long long	time_since_meal;
	long		time_to_sleep;
	long		time_to_die;
	bool		is_urgent;

	pthread_mutex_lock(&philo->data->data_mutex);
	time_to_sleep = philo->data->time_to_sleep;
	time_to_die = philo->data->time_to_die;
	pthread_mutex_unlock(&philo->data->data_mutex);
	print_status(philo, SLEEPING);
	custom_sleep(time_to_sleep);
	print_status(philo, THINKING);
	time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
	is_urgent = (time_since_meal > time_to_die - 150);
	if (!is_urgent && philo->id % 2 == 0)
		custom_sleep(5);
}
