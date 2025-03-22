/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:11 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/22 13:49:26 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static void stagger_start(t_philo *philo)
{
    int num_philos;
    long time_to_eat;
    
    pthread_mutex_lock(&philo->data->data_mutex);
    num_philos = philo->data->num_of_philos;
    time_to_eat = philo->data->time_to_eat;
    pthread_mutex_unlock(&philo->data->data_mutex);
    
    if (num_philos > 100)
    {
        if (philo->id % 2 == 0)
            custom_sleep(time_to_eat / 2);
        else
            custom_sleep((philo->id % 10) * 5);
    }
    else if (philo->id % 2 == 0)
        custom_sleep(10);
}

static void update_urgency(t_philo *philo, bool *is_urgent)
{
    long long time_since_meal;
    long long time_to_die;
    
    pthread_mutex_lock(&philo->data->data_mutex);
    time_to_die = philo->data->time_to_die;
    pthread_mutex_unlock(&philo->data->data_mutex);
    
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    *is_urgent = (time_since_meal > time_to_die - 100);
}

static void handle_backoff(t_philo *philo, bool is_urgent)
{
    long long time_since_meal;
    long long time_to_die;
    bool is_critical;
    bool is_emergency;
    
    pthread_mutex_lock(&philo->data->data_mutex);
    time_to_die = philo->data->time_to_die;
    pthread_mutex_unlock(&philo->data->data_mutex);
    
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    is_critical = (time_since_meal > time_to_die - 50);
    is_emergency = (time_since_meal > time_to_die - 15);
    
    if (is_emergency)
        usleep(1);
    else if (is_critical)
        usleep(5);
    else if (is_urgent)
        usleep(20);
    else
        usleep((philo->id % 3 + 1) * 100);
}

static bool try_acquire_forks(t_philo *philo)
{
    unsigned int attempts;
    bool is_urgent;

    attempts = 0;
    update_urgency(philo, &is_urgent);
    while (!take_forks(philo) && !get_simulation_end(philo->data))
    {
        attempts++;
        handle_backoff(philo, is_urgent);
        if (attempts % 5 == 0)
            update_urgency(philo, &is_urgent);
    }
    return (!get_simulation_end(philo->data));
}

static bool check_meal_limit(t_philo *philo)
{
    bool should_exit;
    int num_meals;
    
    pthread_mutex_lock(&philo->data->data_mutex);
    num_meals = philo->data->num_of_meals;
    pthread_mutex_unlock(&philo->data->data_mutex);
    
    if (num_meals == -1)
        return (false);
        
    pthread_mutex_lock(&philo->data->meal_mutex);
    should_exit = (philo->meals_eaten >= num_meals);
    pthread_mutex_unlock(&philo->data->meal_mutex);
    
    return (should_exit);
}

static void handle_post_eating(t_philo *philo)
{
    long long time_since_meal;
    long long time_to_die;
    bool is_urgent;
    
    pthread_mutex_lock(&philo->data->data_mutex);
    time_to_die = philo->data->time_to_die;
    pthread_mutex_unlock(&philo->data->data_mutex);
    
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    is_urgent = (time_since_meal > time_to_die - 100);
    
    if (time_since_meal > time_to_die - 150)
        print_status(philo, THINKING);
    else
        sleep_think(philo);
        
    time_since_meal = time_since_start(philo->data) - get_last_meal_time(philo);
    
    if (time_since_meal <= time_to_die - 100 && !is_urgent)
        usleep((philo->id % 5) * 50);
}

void *philosopher_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    stagger_start(philo);
    while (!get_simulation_end(philo->data))
    {
        if (!try_acquire_forks(philo))
            break;
        eat(philo);
        if (check_meal_limit(philo))
            break;
        handle_post_eating(philo);
    }
    return (NULL);
}

static void init_meal_times(t_data *data)
{
    int i;
    int num_philos;
    
    pthread_mutex_lock(&data->data_mutex);
    num_philos = data->num_of_philos;
    pthread_mutex_unlock(&data->data_mutex);
    
    i = 0;
    while (i < num_philos)
    {
        pthread_mutex_lock(&data->meal_mutex);
        data->philos[i].last_meal_time = time_since_start(data);
        pthread_mutex_unlock(&data->meal_mutex);
        i++;
    }
}

static int create_threads(t_data *data)
{
    int i;
    int num_philos;
    
    pthread_mutex_lock(&data->data_mutex);
    num_philos = data->num_of_philos;
    pthread_mutex_unlock(&data->data_mutex);
    
    i = 0;
    while (i < num_philos)
    {
        if (pthread_create(&data->philos[i].thread, NULL, &philosopher_routine,
                &data->philos[i]) != 0)
            return (1);
        i++;
    }
    usleep(1000);
    if (pthread_create(&data->monitor_thread, NULL, &monitor_routine, data) != 0)
        return (1);
    // Remove the pthread_detach line
    return (0);
}

int start_simulation(t_data *data)
{
    data->start_time = get_time_in_ms();
    init_meal_times(data);
    return (create_threads(data));
}

void join_philosophers(t_data *data)
{
    int i;
    int num_philos;
    
    pthread_mutex_lock(&data->data_mutex);
    num_philos = data->num_of_philos;
    pthread_mutex_unlock(&data->data_mutex);
    
    i = 0;
    while (i < num_philos)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
	pthread_join(data->monitor_thread, NULL);
}
