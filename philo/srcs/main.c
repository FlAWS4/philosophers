/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 20:43:00 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/24 17:15:01 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int start_simulation(t_data *data)
{
	data->start_time = get_time_in_ms();
	init_meal_times(data);
	return (create_threads(data));
}

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

int	error_exit(char *msg)
{
	printf("Error: %s\n", msg);
	return (1);
}

int	free_resources(t_data *data)
{
	int	i;

	if (data->philos)
		free(data->philos);
	if (data->forks)
	{
		i = 0;
		while (i < data->philos_nb)
		{
			pthread_mutex_destroy(&data->forks[i].mutex);
			i++;
		}
		free(data->forks);
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->end_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	pthread_mutex_destroy(&data->data_mutex);
	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;

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
	pthread_mutex_lock(&data.end_mutex);
	data.simulation_end = true;
	pthread_mutex_unlock(&data.end_mutex);
	usleep(1000);
	free_resources(&data);
	return (0);
}
