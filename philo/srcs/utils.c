/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: my42 <my42@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:30 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/20 13:52:28 by my42             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

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

int error_exit(char *msg)
{
    printf("Error: %s\n", msg);
    return (1);
}

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

bool parse_args(int argc, char **argv, t_data *data)
{
    int i;
    int value;
    
    (void)data;
    i = 1;
    while (i < argc)
    {
        value = ft_atoi(argv[i]);
        if (value <= 0)
            return (false);
        
        if (i == 1 && value > 200)
            return (false);
        
        i++;
    }
    
    return (true);
}
long long get_time_in_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void custom_sleep(long long time)
{
    long long start;
    
    start = get_time_in_ms();
    while (get_time_in_ms() - start < time)
        usleep(100);
}

long long time_since_start(t_data *data)
{
    return (get_time_in_ms() - data->start_time);
}

void update_last_meal(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->meal_mutexes);
    philo->last_meal_time = time_since_start(philo->data);
    pthread_mutex_unlock(&philo->data->meal_mutexes);
}

long long get_last_meal_time(t_philo *philo)
{
    long long time;
    
    pthread_mutex_lock(&philo->data->meal_mutexes);
    time = philo->last_meal_time;
    pthread_mutex_unlock(&philo->data->meal_mutexes);
    
    return (time);
}
