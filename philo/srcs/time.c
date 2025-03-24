/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 13:50:54 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/24 17:57:01 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (0);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	custom_sleep(int milliseconds)
{
	long long	start_time;
	long long	elapsed;
	long long	remaining;

	start_time = get_time_in_ms();
	while (1)
	{
		elapsed = get_time_in_ms() - start_time;
		if (elapsed >= milliseconds)
			break ;
		remaining = milliseconds - elapsed;
		if (remaining > 10)
			usleep(remaining * 800);
		else
			usleep(200);
	}
}

long long	time_since_start(t_data *data)
{
	return (get_time_in_ms() - data->start_time);
}

void	update_last_meal(t_philo *philo)
{
	long long	current;

	current = time_since_start(philo->data);
	pthread_mutex_lock(&philo->data->meal_mutex);
	if (current == 0)
		current = 1;
	philo->last_meal_time = current;
	pthread_mutex_unlock(&philo->data->meal_mutex);
}

long long	get_last_meal_time(t_philo *philo)
{
	long long	time;

	pthread_mutex_lock(&philo->data->meal_mutex);
	time = philo->last_meal_time;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	return (time);
}
