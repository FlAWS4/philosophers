/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:05 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/22 13:49:06 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	*monitor_routine(void *arg)
{
    t_data	*data;

    data = (t_data *)arg;
    usleep(10000);
    while (!get_simulation_end(data))
    {
        if (check_if_simulation_should_end(data))
            set_simulation_end(data);
        usleep(500);        
    }
    return (NULL);
}

static bool check_philosopher_death(t_data *data, int i)
{
    long long current_time;
    long long meal_time;
    long long time_since_last_meal;
    long long time_to_die;
    
    pthread_mutex_lock(&data->data_mutex);
    time_to_die = data->time_to_die;
    pthread_mutex_unlock(&data->data_mutex);
    
    current_time = time_since_start(data);
    pthread_mutex_lock(&data->meal_mutex);
    meal_time = data->philos[i].last_meal_time;
    time_since_last_meal = current_time - meal_time;
    pthread_mutex_unlock(&data->meal_mutex);
    
    if (time_since_last_meal > time_to_die)
    {
        pthread_mutex_lock(&data->print_mutex);
        if (!get_simulation_end(data))
            printf("%lld %d died\n", current_time, data->philos[i].id);
        pthread_mutex_unlock(&data->print_mutex);
        return (true);
    }
    return (false);
}

static bool check_all_finished_eating(t_data *data, int finished_eating)
{
    int num_meals;
    int num_philos;
    
    pthread_mutex_lock(&data->data_mutex);
    num_meals = data->num_of_meals;
    num_philos = data->num_of_philos;
    pthread_mutex_unlock(&data->data_mutex);
    
    if (num_meals == -1)
        return (false);
    if (finished_eating != num_philos)
        return (false);
        
    pthread_mutex_lock(&data->print_mutex);
    if (!get_simulation_end(data))
        printf("All philosophers have eaten %d times, simulation complete.\n", 
            num_meals);
    pthread_mutex_unlock(&data->print_mutex);
    return (true);
}

bool	check_if_simulation_should_end(t_data *data)
{
    int	i;
    int	finished_eating;
    int	num_philos;
    int	num_meals;

    finished_eating = 0;
    i = 0;
    
    pthread_mutex_lock(&data->data_mutex);
    num_philos = data->num_of_philos;
    num_meals = data->num_of_meals;
    pthread_mutex_unlock(&data->data_mutex);
    
    while (i < num_philos)
    {
        if (check_philosopher_death(data, i))
            return (true);
            
        pthread_mutex_lock(&data->meal_mutex);
        if (num_meals != -1 && 
            data->philos[i].meals_eaten >= num_meals)
            finished_eating++;
        pthread_mutex_unlock(&data->meal_mutex);
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
