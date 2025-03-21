/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:05 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 18:09:20 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void *monitor_routine(void *arg)
{
    t_data *data;
    
    data = (t_data *)arg;
    // Allow philosophers to initialize before starting monitoring
    usleep(10000);  // 10ms delay to ensure all philosophers are ready
    
    while (!get_simulation_end(data))
    {
        if (check_if_simulation_should_end(data))
            set_simulation_end(data);
        usleep(500); // More frequent checks (0.5ms) for higher precision
    }
    
    return (NULL);
}

bool check_if_simulation_should_end(t_data *data)
{
    int i;
    int finished_eating;
    long long current_time;
    long long meal_time;
    long long time_since_last_meal;
    
    finished_eating = 0;
    i = 0;
    while (i < data->num_of_philos)
    {
        // Get current time once for each philosopher
        current_time = time_since_start(data);
        
        // Use mutex protection for critical section
        pthread_mutex_lock(&data->meal_mutexes);
        meal_time = data->philos[i].last_meal_time;
        
        // Calculate time difference
        time_since_last_meal = current_time - meal_time;
        
        // DEBUG message to help find the issue
        if (time_since_last_meal > data->time_to_die - 10)
        {
            // Approaching death threshold - log for debugging
            printf("DEBUG: Philo %d close to death: current=%lld, last=%lld, diff=%lld, limit=%lld\n",
                data->philos[i].id, current_time, meal_time, 
                time_since_last_meal, data->time_to_die);
        }
        
        // Check for death
        if (time_since_last_meal > data->time_to_die)
        {
            pthread_mutex_unlock(&data->meal_mutexes);
            
            pthread_mutex_lock(&data->print_mutex);
            if (!get_simulation_end(data))
            {
                printf("%lld %d died\n", current_time, data->philos[i].id);
            }
            pthread_mutex_unlock(&data->print_mutex);
            
            return (true);
        }
        
        // Check meals eaten
        if (data->num_of_meals != -1 && 
            data->philos[i].meals_eaten >= data->num_of_meals)
            finished_eating++;
            
        pthread_mutex_unlock(&data->meal_mutexes);
        i++;
    }
    
    // Check if all philosophers have eaten enough
    if (data->num_of_meals != -1 && finished_eating == data->num_of_philos)
    {
        pthread_mutex_lock(&data->print_mutex);
        if (!get_simulation_end(data))
        {
            printf("All philosophers have eaten %d times, simulation complete.\n", 
                   data->num_of_meals);
        }
        pthread_mutex_unlock(&data->print_mutex);
        return (true);
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
