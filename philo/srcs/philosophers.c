/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:11 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/20 16:04:16 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void *philosopher_routine(void *arg)
{
    t_philo *philo;
    
    philo = (t_philo *)arg;
    
    if (philo->id % 2 == 0)
        custom_sleep(philo->data->time_to_eat / 2);
    
    while (!get_simulation_end(philo->data))
    {
        if (take_forks(philo) == false)
            continue ; // Skip to next iteration if couldn't take forks
        
        eat(philo);
        sleep_think(philo);
        
        if (philo->data->num_of_meals != -1 && 
            philo->meals_eaten >= philo->data->num_of_meals)
            break ;
    }
    
    return (NULL);
}

int start_simulation(t_data *data)
{
    int i;
    
    // Record start time BEFORE creating threads
    data->start_time = get_time_in_ms();
    // Initialize all philosophers' last_meal_time to the start time
    i = 0;
    while (i < data->num_of_philos)
    {
        pthread_mutex_lock(&data->meal_mutexes);
        data->philos[i].last_meal_time = 0;
        pthread_mutex_unlock(&data->meal_mutexes);
        i++;
    }
    // Create philosopher threads
    i = 0;
    while (i < data->num_of_philos)
    {
        if (pthread_create(&data->philos[i].thread, NULL,
                          philosopher_routine, &data->philos[i]) != 0)
            return (1);
        // Small delay between thread creation for better scheduling
        usleep(100);
        i++;
    }
    // Give philosophers a bit more head start
    usleep(2000);
    if (pthread_create(&data->monitor_thread, NULL, monitor_routine, data) != 0)
        return (1);
    // DO NOT detach the monitor thread
    return (0);
}

void join_philosophers(t_data *data)
{
    int i;
    
    // Signal all threads to terminate
    pthread_mutex_lock(&data->end_mutex);
    data->simulation_end = 1;
    pthread_mutex_unlock(&data->end_mutex);
    
    // Wait for all philosopher threads to terminate
    i = 0;
    while (i < data->num_of_philos)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
    
    // Join the monitor thread as well
    pthread_join(data->monitor_thread, NULL);
    
    // Add a small delay to ensure threads are fully done
    usleep(1000);
}
