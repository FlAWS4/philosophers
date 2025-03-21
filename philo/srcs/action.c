/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:11:58 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 23:01:00 by mshariar         ###   ########.fr       */
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

static bool	take_left_first(t_philo *philo)
{
    pthread_mutex_lock(&philo->left_fork->mutex);
    print_status(philo, FORK);
    if (get_simulation_end(philo->data))
    {
        pthread_mutex_unlock(&philo->left_fork->mutex);
        return (false);
    }
    if (philo->data->num_of_philos == 1)
        return (handle_single_philo(philo));
    pthread_mutex_lock(&philo->right_fork->mutex);
    print_status(philo, FORK);
    return (true);
}

static bool	take_right_first(t_philo *philo)
{
    pthread_mutex_lock(&philo->right_fork->mutex);
    print_status(philo, FORK);
    if (get_simulation_end(philo->data))
    {
        pthread_mutex_unlock(&philo->right_fork->mutex);
        return (false);
    }
    pthread_mutex_lock(&philo->left_fork->mutex);
    print_status(philo, FORK);
    return (true);
}

bool	take_forks(t_philo *philo)
{
    long long	time_since_meal;
    bool		is_urgent;
    bool		take_left_first_flag;
    bool		success;

    if (get_simulation_end(philo->data))
        return (false);
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    is_urgent = (time_since_meal > philo->data->time_to_die - 100);
    take_left_first_flag = (philo->id % 2 == 1);
    if (is_urgent)
        take_left_first_flag = !(philo->id % 2 == 1);
    if (take_left_first_flag)
        success = take_left_first(philo);
    else
        success = take_right_first(philo);
    if (!success || get_simulation_end(philo->data))
    {
        pthread_mutex_unlock(&philo->left_fork->mutex);
        pthread_mutex_unlock(&philo->right_fork->mutex);
        return (false);
    }
    return (true);
}

void	eat(t_philo *philo)
{
    update_last_meal(philo);
    print_status(philo, EATING);
    custom_sleep(philo->data->time_to_eat);
    pthread_mutex_lock(&philo->data->meal_mutexes);
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->data->meal_mutexes);
    pthread_mutex_unlock(&philo->left_fork->mutex);
    pthread_mutex_unlock(&philo->right_fork->mutex);
}

void	sleep_think(t_philo *philo)
{
    long long	time_since_meal;
    bool		is_urgent;

    print_status(philo, SLEEPING);
    custom_sleep(philo->data->time_to_sleep);
    print_status(philo, THINKING);
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    is_urgent = (time_since_meal > philo->data->time_to_die - 150);                 //pareil ici
    if (!is_urgent && philo->id % 2 == 0)
        custom_sleep(5);
}
