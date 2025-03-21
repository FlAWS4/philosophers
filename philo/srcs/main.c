/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 20:43:00 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/21 21:26:40 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	free_resources(t_data *data)
{
	int	i;

	if (data->philos)
		free(data->philos);
	if (data->forks)
	{
		i = 0;
		while (i < data->num_of_philos)
		{
			pthread_mutex_destroy(&data->forks[i].mutex);
			i++;
		}
		free(data->forks);
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->end_mutex);
	pthread_mutex_destroy(&data->meal_mutexes);
	return (0);
}

int main(int argc, char **argv)
{
    t_data data;

    memset(&data, 0, sizeof(t_data));
    if (argc != 5 && argc != 6)
        return (error_exit("Invalid number of arguments"));
    if (!parse_args(argc, argv, &data))
        return (error_exit("Invalid arguments"));
    if (init_data(&data, argc, argv) != 0)
        return (error_exit("Initialization error"));
    if (start_simulation(&data) != 0)
    {
        free_resources(&data);
        return (error_exit("Failed to start simulation"));
    }
    join_philosophers(&data);
    usleep(1000);
    
    free_resources(&data);
    return (0);
}
