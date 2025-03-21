/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:05 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 22:57:08 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	*monitor_routine(void *arg)
{
    t_data	*data;

    data = (t_data *)arg;
    usleep(10000);                          //pourquoi 10ms?
    while (!get_simulation_end(data))
    {
        if (check_if_simulation_should_end(data))
            set_simulation_end(data);
        usleep(500);        
    }
    return (NULL);
}

static bool	check_philosopher_death(t_data *data, int i)
{
    long long	current_time;
    long long	meal_time;
    long long	time_since_last_meal;
    bool		should_end;

    should_end = false;
    current_time = time_since_start(data);
    pthread_mutex_lock(&data->meal_mutexes);
    meal_time = data->philos[i].last_meal_time;
    time_since_last_meal = current_time - meal_time;
    if (time_since_last_meal > data->time_to_die)
    {
        pthread_mutex_unlock(&data->meal_mutexes);
        pthread_mutex_lock(&data->print_mutex);
        if (!get_simulation_end(data))
            printf("%lld %d died\n", current_time, data->philos[i].id);
        pthread_mutex_unlock(&data->print_mutex);
        return (true);
    }
    pthread_mutex_unlock(&data->meal_mutexes);
    return (false);
}

static bool	check_all_finished_eating(t_data *data, int finished_eating)
{
    if (data->num_of_meals == -1)                                   //il faut proteger l'acces a data->num_of_meals avec mutex
        return (false);
    if (finished_eating != data->num_of_philos)                     //pareil ici
        return (false);
    pthread_mutex_lock(&data->print_mutex);
    if (!get_simulation_end(data))
    {
        printf("All philosophers have eaten %d times, simulation complete.\n", 
            data->num_of_meals);
    }
    pthread_mutex_unlock(&data->print_mutex);
    return (true);
}

bool	check_if_simulation_should_end(t_data *data)
{
    int	i;
    int	finished_eating;

    finished_eating = 0;
    i = 0;
    while (i < data->num_of_philos)
    {
        if (check_philosopher_death(data, i))
            return (true);
        pthread_mutex_lock(&data->meal_mutexes);
        if (data->num_of_meals != -1 && 
            data->philos[i].meals_eaten >= data->num_of_meals)
            finished_eating++;
        pthread_mutex_unlock(&data->meal_mutexes);
        i++;
    }
    return (check_all_finished_eating(data, finished_eating));
}

bool	get_simulation_end(t_data *data)
{
    bool	end;

    pthread_mutex_lock(&data->end_mutex);
    end = data->simulation_end;
    pthread_mutex_unlock(&data->end_mutex);
    return (end);
}

void	set_simulation_end(t_data *data)
{
    pthread_mutex_lock(&data->end_mutex);
    data->simulation_end = true;
    pthread_mutex_unlock(&data->end_mutex);
}
