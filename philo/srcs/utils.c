/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:30 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 18:07:16 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/**
 * Convert string to integer with overflow checking
 */
int ft_atoi(const char *str)
{
    int     i;
    int     sign;
    long    result;

    i = 0;
    sign = 1;
    result = 0;
    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        if (result * sign > INT_MAX)
            return (INT_MAX);
        if (result * sign < INT_MIN)
            return (INT_MIN);
        i++;
    }
    return (result * sign);
}

/**
 * Print error message and return failure code
 */
int error_exit(char *msg)
{
    printf("Error: %s\n", msg);
    return (1);
}

/**
 * Thread-safe status printer with simulation end check
 */
void print_status(t_philo *philo, char *status)
{
    long long current_time;
    
    pthread_mutex_lock(&philo->data->print_mutex);
    if (!get_simulation_end(philo->data))
    {
        current_time = time_since_start(philo->data);
        printf("%lld %d %s\n", current_time, philo->id, status);
    }
    pthread_mutex_unlock(&philo->data->print_mutex);
}

/**
 * Validate command line arguments and store in data structure
 * Note: This should be the ONLY place where cmd arguments are parsed
 */
bool parse_args(int argc, char **argv, t_data *data)
{
    int i;
    int value;
    
    i = 1;
    while (i < argc)
    {
        // Check if current argument is a valid positive number
        if (!argv[i][0] || !ft_isdigit(argv[i][0]))
            return (false);
            
        value = ft_atoi(argv[i]);
        if (value <= 0)
            return (false);
        
        // Store values directly in data structure
        if (i == 1)
        {
            if (value > 200) // Max philosopher limit
                return (false);
            data->num_of_philos = value;
        }
        else if (i == 2)
            data->time_to_die = value;
        else if (i == 3)
            data->time_to_eat = value;
        else if (i == 4)
            data->time_to_sleep = value;
        else if (i == 5)
            data->num_of_meals = value;
        
        i++;
    }
    
    // Default value for optional argument
    if (argc == 5)
        data->num_of_meals = -1;
    
    return (true);
}

/**
 * Get current time in milliseconds
 */
long long get_time_in_ms(void)
{
    struct timeval tv;
    
    if (gettimeofday(&tv, NULL) != 0)
        return (0); // Error handling
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/**
 * More efficient sleep function that reduces CPU usage
 */
void custom_sleep(int milliseconds)
{
    long long start_time;
    long long elapsed;
    long long remaining;
    
    start_time = get_time_in_ms();
    while (1)
    {
        elapsed = get_time_in_ms() - start_time;
        if (elapsed >= milliseconds)
            break;
            
        // Adaptive sleep approach - sleep longer when we have more time
        remaining = milliseconds - elapsed;
        if (remaining > 10)
            usleep(remaining * 800); // Sleep for 80% of remaining time
        else
            usleep(200); // Short sleep for fine-grained control
    }
}

/**
 * Calculate time since simulation start
 */
long long time_since_start(t_data *data)
{
    return (get_time_in_ms() - data->start_time);
}

/**
 * CRITICAL: This is the ONLY function that should update last_meal_time
 * Call this function instead of directly modifying the variable
 */
void update_last_meal(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->meal_mutexes);
    philo->last_meal_time = time_since_start(philo->data);
    pthread_mutex_unlock(&philo->data->meal_mutexes);
}

/**
 * Thread-safe getter for last_meal_time
 */
long long get_last_meal_time(t_philo *philo)
{
    long long time;
    
    pthread_mutex_lock(&philo->data->meal_mutexes);
    time = philo->last_meal_time;
    pthread_mutex_unlock(&philo->data->meal_mutexes);
    
    return (time);
}

/**
 * Helper to check if a character is a digit
 */
bool ft_isdigit(char c)
{
    return (c >= '0' && c <= '9');
}
