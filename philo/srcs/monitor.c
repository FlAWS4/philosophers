/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: my42 <my42@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:05 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 16:21:16 by my42             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void *monitor_routine(void *arg)
{
    t_data *data;
    
    data = (t_data *)arg;
    // Allow philosophers to initialize before starting monitoring
    usleep(5000);  // Increased delay to ensure all philosophers are ready
    
    while (!get_simulation_end(data))
    {
        if (check_if_simulation_should_end(data))
            set_simulation_end(data);
        usleep(1000); // Check every 1ms
    }
    
    return (NULL);
}

bool check_if_simulation_should_end(t_data *data)
{
    int i;
    int finished_eating;
    long long current_time;
    long long time_since_last_meal;
    
    // Get the current time once at the start for consistency
    current_time = time_since_start(data);
    
    finished_eating = 0;
    i = 0;
    while (i < data->num_of_philos)
    {
        pthread_mutex_lock(&data->meal_mutexes);
        
        // Calculate how long it's been since the philosopher's last meal
        time_since_last_meal = current_time - data->philos[i].last_meal_time;
        
        // Check if philosopher has died
        if (time_since_last_meal >= data->time_to_die)
        {
            // Release mutex before performing more operations
            pthread_mutex_unlock(&data->meal_mutexes);
            
            // Double-check after a small delay to prevent false deaths
            usleep(1000);
            
            // Get fresh current time for the second check
            current_time = time_since_start(data);
            
            // Re-lock the mutex for the second check
            pthread_mutex_lock(&data->meal_mutexes);
            time_since_last_meal = current_time - data->philos[i].last_meal_time;
            
            // Only report death if still over time_to_die after the delay
            if (time_since_last_meal >= data->time_to_die)
            {
                pthread_mutex_unlock(&data->meal_mutexes);
                
                // Use print_mutex to ensure death is reported atomically
                pthread_mutex_lock(&data->print_mutex);
                if (!get_simulation_end(data)) // Ensure we only report death once
                {
                    printf("%lld %d died\n", current_time, data->philos[i].id);
                }
                pthread_mutex_unlock(&data->print_mutex);
                
                return (true);
            }
            
            pthread_mutex_unlock(&data->meal_mutexes);
        }
        else
        {
            // Check if philosopher has eaten enough meals
            if (data->num_of_meals != -1 && 
                data->philos[i].meals_eaten >= data->num_of_meals)
                finished_eating++;
                
            pthread_mutex_unlock(&data->meal_mutexes);
        }
        
        i++;
    }
    
    // If all philosophers have eaten enough meals
    if (data->num_of_meals != -1 && finished_eating == data->num_of_philos)
    {
        pthread_mutex_lock(&data->print_mutex);
        // Only report all philosophers finished if simulation is still running
        if (!get_simulation_end(data))
        {
            printf("All philosophers have eaten %d times, simulation complete.\n", 
                   data->num_of_meals);
        }
        pthread_mutex_unlock(&data->print_mutex);
        return (true); // This will set simulation_end to true
    }
    
    return (false);
}

bool get_simulation_end(t_data *data)
{
    bool end;
    
    pthread_mutex_lock(&data->end_mutex);
    end = data->simulation_end;
    pthread_mutex_unlock(&data->end_mutex);
    
    return (end);
}

void set_simulation_end(t_data *data)
{
    pthread_mutex_lock(&data->end_mutex);
    data->simulation_end = true;
    pthread_mutex_unlock(&data->end_mutex);
}
