/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:11 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/24 17:49:04 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	update_urgency(t_philo *philo, bool *is_urgent)
{
	long long	time_since_meal;
	long long	time_to_die;

	pthread_mutex_lock(&philo->data->data_mutex);
	time_to_die = philo->data->time_to_die;
	pthread_mutex_unlock(&philo->data->data_mutex);
	time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
	*is_urgent = (time_since_meal > time_to_die - 100);
}

void	handle_backoff(t_philo *philo, bool is_urgent)
{
	long long	time_since_meal;
	long long	time_to_die;
	bool		is_critical;
	bool		is_emergency;

	pthread_mutex_lock(&philo->data->data_mutex);
	time_to_die = philo->data->time_to_die;
	pthread_mutex_unlock(&philo->data->data_mutex);
	time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
	is_critical = (time_since_meal > time_to_die - 50);
	is_emergency = (time_since_meal > time_to_die - 15);
	if (is_emergency)
		usleep(1);
	else if (is_critical)
		usleep(5);
	else if (is_urgent)
		usleep(20);
	else
		usleep((philo->id % 3 + 1) * 100);
}

void	handle_post_eating(t_philo *philo)
{
	long long	time_since_meal;
	long long	time_to_die;
	bool		is_urgent;

	pthread_mutex_lock(&philo->data->data_mutex);
	time_to_die = philo->data->time_to_die;
	pthread_mutex_unlock(&philo->data->data_mutex);
	time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
	is_urgent = (time_since_meal > time_to_die - 100);
	if (time_since_meal > time_to_die - 150)
		print_status(philo, THINKING);
	else
		sleep_think(philo);
	time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
	if (time_since_meal <= time_to_die - 100 && !is_urgent)
		usleep((philo->id % 5) * 50);
}

int	create_threads(t_data *data)
{
	int	i;
	int	num_philos;

	pthread_mutex_lock(&data->data_mutex);
	num_philos = data->philos_nb;
	pthread_mutex_unlock(&data->data_mutex);
	i = 0;
	while (i < num_philos)
	{
		if (pthread_create(&data->philos[i].thread, NULL, &philosopher_routine,
				&data->philos[i]) != 0)
			return (1);
		i++;
	}
	usleep(1000);
	if (pthread_create(&data->monitor_thread, NULL,
			&monitor_routine, data) != 0)
		return (1);
	return (0);
}

void	join_philosophers(t_data *data)
{
	int	i;
	int	num_philos;

	pthread_mutex_lock(&data->data_mutex);
	num_philos = data->philos_nb;
	pthread_mutex_unlock(&data->data_mutex);
	i = 0;
	while (i < num_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
	pthread_join(data->monitor_thread, NULL);
}
