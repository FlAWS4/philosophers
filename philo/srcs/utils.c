/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:12:30 by mshariar          #+#    #+#             */
/*   Updated: 2025/03/28 22:05:28 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	ft_atoi(const char *str)
{
	int		i;
	int		sign;
	long	result;

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

void	print_status(t_philo *philo, char *status)
{
	long long	current_time;

	pthread_mutex_lock(&philo->data->print_mutex);
	if (!get_simulation_end(philo->data))
	{
		current_time = time_since_start(philo->data);
		printf("%lld %d %s\n", current_time, philo->id, status);
	}
	pthread_mutex_unlock(&philo->data->print_mutex);
}

static bool	validate_arg_value(char **argv, int i, int value, t_data *data)
{
	int	j;

	if (!argv[i][0] || value <= 0)
		return (false);
	j = -1;
	while (argv[i][j++])
	{
		if (!ft_isdigit(argv[i][j]))
			return (false);
	}
	if (i == 1)
	{
		if (value > 200)
			return (false);
		data->philos_nb = value;
	}
	else if (i == 2)
		data->time_to_die = value;
	else if (i == 3)
		data->time_to_eat = value;
	else if (i == 4)
		data->time_to_sleep = value;
	else if (i == 5)
		data->num_of_meals = value;
	return (true);
}

bool	parse_args(int argc, char **argv, t_data *data)
{
	int	i;
	int	value;

	i = 1;
	while (i < argc)
	{
		value = ft_atoi(argv[i]);
		if (!validate_arg_value(argv, i, value, data))
			return (false);
		i++;
	}
	if (argc == 5)
		data->num_of_meals = -1;
	return (true);
}

bool	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}
