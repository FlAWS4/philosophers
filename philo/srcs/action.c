/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:11:58 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 17:56:21 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

bool take_forks(t_philo *philo)
{
    // Check if simulation ended before trying to take forks
    if (get_simulation_end(philo->data))
        return (false);
        
    // For odd philosophers, take left fork first
    if (philo->id % 2)
    {
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_status(philo, FORK);
        
        // Check if simulation ended after taking first fork
        if (get_simulation_end(philo->data))
        {
            pthread_mutex_unlock(&philo->left_fork->mutex);
            return (false);
        }
        
        // Special case for single philosopher
        if (philo->data->num_of_philos == 1)
        {
            // Wait until simulation ends (will die from starvation)
            while (!get_simulation_end(philo->data))
                usleep(1000);
            pthread_mutex_unlock(&philo->left_fork->mutex);
            return (false);
        }
        
        pthread_mutex_lock(&philo->right_fork->mutex);
        print_status(philo, FORK);
    }
    else
    {
        // Same logic for even philosophers
        pthread_mutex_lock(&philo->right_fork->mutex);
        print_status(philo, FORK);
        
        if (get_simulation_end(philo->data))
        {
            pthread_mutex_unlock(&philo->right_fork->mutex);
            return (false);
        }
        
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_status(philo, FORK);
    }
    
    // Check one more time in case simulation ended while getting both forks
    if (get_simulation_end(philo->data))
    {
        pthread_mutex_unlock(&philo->left_fork->mutex);
        pthread_mutex_unlock(&philo->right_fork->mutex);
        return (false);
    }
    
    // Do NOT update meal time here - moved to eat() function
    return (true);
}

void eat(t_philo *philo)
{
    // CRITICAL SECTION: Update meal time at the start of eating
    // This is now the ONLY place where last_meal_time is updated
    pthread_mutex_lock(&philo->data->meal_mutexes);
    philo->last_meal_time = time_since_start(philo->data);
    pthread_mutex_unlock(&philo->data->meal_mutexes);
    
    print_status(philo, EATING);
    
    // Eat for the specified time
    custom_sleep(philo->data->time_to_eat);
    
    // Update meal count BEFORE releasing forks
    pthread_mutex_lock(&philo->data->meal_mutexes);
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->data->meal_mutexes);
    
    // Release forks - ensure we do this atomically to prevent deadlocks
    pthread_mutex_unlock(&philo->left_fork->mutex);
    pthread_mutex_unlock(&philo->right_fork->mutex);
}

void sleep_think(t_philo *philo)
{
    print_status(philo, SLEEPING);
    custom_sleep(philo->data->time_to_sleep);
    print_status(philo, THINKING);
    
    // Add a small delay for even philosophers to break symmetry
    if (philo->id % 2 == 0)
        custom_sleep(5);
}
