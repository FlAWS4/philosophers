/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:11 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 17:59:46 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void *philosopher_routine(void *arg)
{
    t_philo *philo;
    
    philo = (t_philo *)arg;
    
    // NO NEED to set last_meal_time here - already set in start_simulation
    // This avoids the double initialization problem
    
    // Stagger start times to avoid all philosophers trying to eat at once
    if (philo->data->num_of_philos > 100)
    {
        // For large philosopher counts, use more aggressive staggering
        if (philo->id % 2 == 0)
            custom_sleep(philo->data->time_to_eat / 2);
        else
            custom_sleep((philo->id % 10) * 5); // Scaled delay for odd IDs
    }
    else if (philo->id % 2 == 0)
        custom_sleep(philo->data->time_to_eat / 4);
    
    // Main philosopher life cycle
    while (!get_simulation_end(philo->data))
    {
        // Try to get forks - with intelligent backoff if unsuccessful
        if (!take_forks(philo))
        {
            if (get_simulation_end(philo->data))
                break;
                
            // Dynamic backoff based on philosopher ID
            usleep((philo->id % 3 + 1) * 300); // Shorter backoff for better reactivity
            continue;
        }
        
        // Eat (meal time is updated INSIDE the eat() function, not in take_forks)
        eat(philo);
        
        // Check meal limit after eating
        if (philo->data->num_of_meals != -1)
        {
            pthread_mutex_lock(&philo->data->meal_mutexes);
            if (philo->meals_eaten >= philo->data->num_of_meals)
            {
                pthread_mutex_unlock(&philo->data->meal_mutexes);
                break;
            }
            pthread_mutex_unlock(&philo->data->meal_mutexes);
        }
        
        // Everyone sleeps and thinks the same way
        sleep_think(philo);
        
        // Very small delay to break symmetry, with a shorter cycle
        usleep((philo->id % 5) * 200);
    }
    
    return (NULL);
}

int start_simulation(t_data *data)
{
    int i;
    pthread_t monitor;
    
    // Set the start time for the entire simulation
    data->start_time = get_time_in_ms();
    
    // Initialize all philosophers' last meal times to the start time
    i = 0;
    while (i < data->num_of_philos)
    {
        pthread_mutex_lock(&data->meal_mutexes);
        // Set to current time instead of 0 to give philosophers proper time to start
        data->philos[i].last_meal_time = time_since_start(data);
        pthread_mutex_unlock(&data->meal_mutexes);
        i++;
    }
    
    // Start philosopher threads FIRST
    i = 0;
    while (i < data->num_of_philos)
    {
        if (pthread_create(&data->philos[i].thread, NULL, &philosopher_routine,
                         &data->philos[i]) != 0)
            return (1);
        i++;
    }
    
    // Small delay to let philosophers start before monitoring
    usleep(1000);
    
    // Create the monitor thread AFTER philosophers have started
    if (pthread_create(&monitor, NULL, &monitor_routine, data) != 0)
        return (1);
    pthread_detach(monitor);
    
    return (0);
}

void join_philosophers(t_data *data)
{
    int i;
    
    i = 0;
    while (i < data->num_of_philos)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
}
