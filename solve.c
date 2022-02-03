/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akoykka <akoykka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 11:25:05 by akoykka           #+#    #+#             */
/*   Updated: 2022/02/03 18:57:07 by akoykka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "./libft/includes/libft.h"
#include <fcntl.h>
#define FAIL 1
#define SUCCESS 0
#define NOT_FOUND 404
#define FOUND 1

typedef struct s_tetromino
{
	char			*square;
	char			**tetrominos;
	int				sidelen;
	int				tetromino_amount;
	char			*answer;
	int				letter;
}				t_tetromino;

void	copy_except(char *dest, char *src, int c)
{
	int	i;

	while (*src)
	{
		if (*src != c)
		{
			*dest = *src;
			dest++;
		}
	src++;
	}
	return ;
}

int	count_and_save_tetrominos(char *filename, char **tetrominos)
{
	ssize_t		r_value;
	char		buf[21 + 1];
	int			amount_of_tetrominos;
	int			fd;

	fd = open(filename, O_RDONLY);
	amount_of_tetrominos = 0;
	ft_memset(buf, '\0', 22);
	r_value = read(fd, &buf, 21);
	while (r_value)
	{
		*tetrominos = (char *)ft_memalloc(16 + 1);
		copy_except(*tetrominos, buf, '\n');
		++tetrominos;
		++amount_of_tetrominos;
		r_value = read(fd, &buf, 21);
	}
	close(fd);
	return (amount_of_tetrominos);
}
void	print_solution(t_tetromino *values)
{
	int	print;
	int	print1;

	print = 0;
	while (values->answer[print])
	{
		print1 = values->sidelen - 1;
		while (print1--)
		{
			ft_putchar(values->answer[print]);
			++print;
		}
		ft_putchar('\n');
	}
	printf("sidelen for answer is %d\n", (values->sidelen - 1));
	printf("strlen is %lu\n", (ft_strlen(values->answer)));
}
/*void	print_progress(t_tetromino *values)
{
	char *progress;
	int loop;
	int x;
	char *temp;
	int = i;

	temp = values->square;
	x = 1;
	progress = ft_memalloc(values->sidelen * values->sidelen + values->sidelen + 1);
	loop = values->sidelen;
	while (loop--)
	{
		temp[i] = 
		temp += values->sidelen;
		progress[values->sidelen * x] = '\n';
		++x;

	}
	printf("%s\n Sidelen %d\n", progress, values->sidelen);
}
*/
char	*makesquare(int sidelen)
{
	char	*square;

	square = (char *)ft_memalloc(sidelen * sidelen + sidelen + 1);
	if (!square)
		exit(1);
	ft_memset(square, '.', sidelen * sidelen);
	return (square);
}

void	undo_last_tetromino(char *square, int letter)
{
	while (*square)
	{
		if (*square == letter)
			*square = '.';
		++square;
	}
}

int	wallcollision(t_tetromino *values)
{
	int	wall_index;

	wall_index = values->sidelen;
	while (values->square[wall_index])
	{
		if (values->square[wall_index] == values->letter
			&& values->square[wall_index - 1] == values->letter)
		{
			undo_last_tetromino(values->square, values->letter);
			return (FAIL);
		}
		wall_index = wall_index + values->sidelen;
	}
	//print_progress(values);
	return (SUCCESS);
}

int	convert_to_square_size(int distance, int sidelen)
{
	int	converted_len;

	if (distance > 1)
	{
		converted_len = distance + (sidelen - 4);
		return (converted_len);
	}
	return (distance);
}

int	get_distance_from_tetromino(char *block, int sidelen)
{
	int	distance;

	distance = 1;
	while (block[distance] != '#')
		++distance;
	return (distance);
}

int	collision(t_tetromino *values, int index)
{
	char	*block;
	int		loop;
	int		dist_to_next;

	values->square[index] = values->letter;
	block = ft_strchr(*values->tetrominos, '#');
	loop = 3;
	while (loop--)
	{
		dist_to_next = get_distance_from_tetromino(block, values->sidelen);
		index = index + convert_to_square_size(dist_to_next, values->sidelen);
		block = block + dist_to_next;
		if (values->square[index] == '.')
			values->square[index] = values->letter;
		else
		{
			undo_last_tetromino(values->square, values->letter);
			return (FAIL);
		}

	}
	return (SUCCESS);
}

void	recursive_solver(t_tetromino *values)
{
	int	i;

	i = 0;
	if (!*(values->tetrominos))
	{
		values->answer = ft_strdup(values->square);
		return ;
	}
	while (values->answer == NULL && values->square[i])
	{
		if (values->square[i] == '.'
			&& collision(values, i) == SUCCESS
			&& wallcollision(values) == SUCCESS)
		{
			++values->letter;
			++values->tetrominos;
			recursive_solver(values);
			--values->letter;
			--values->tetrominos;
			undo_last_tetromino(values->square, values->letter);
		}
		++i;
	}
}

int	count_minimum_sidelen(int tetromino_amount)
{
	int	sqroot_rounded_up;

	sqroot_rounded_up = 0;
	while (sqroot_rounded_up * sqroot_rounded_up <= tetromino_amount * 4)
		++sqroot_rounded_up;
	return (sqroot_rounded_up);
}

t_tetromino	*prepare_struct(char *filename)
{
	t_tetromino	*values;
	int			tetromino_amount;

	values = (t_tetromino *)malloc(sizeof(t_tetromino));
	values->tetrominos = (char **)ft_memalloc(sizeof(char *) * (26 + 1));
	if (!values || !values->tetrominos)
		exit(1);
	tetromino_amount = count_and_save_tetrominos(filename, values->tetrominos);
	values->sidelen = count_minimum_sidelen(tetromino_amount);
	values->answer = NULL;
	values->square = NULL;
	values->letter = 'A';
	return (values);
}

int	main(int argc, char **argv)
{
	int			fd;
	t_tetromino	*values;

	if (argc == 0)
	{
		printf("Wheres the file?\n");
		return (0);
	}
	if (argc != 2)
	{
		printf("USAGE\n");
		return (0);
	}
	values = prepare_struct(argv[1]);
	while (values->answer == NULL)
	{
		ft_strdel(&values->square);
		values->square = makesquare(values->sidelen);
		recursive_solver(values);
		++values->sidelen;
	}
	print_solution(values);
	ft_strdel(values->tetrominos);
	ft_strdel(values->tetrominos);
	ft_strdel(values->tetrominos);
	ft_strdel(values->tetrominos);
	ft_strdel(values->tetrominos);
	ft_strdel(values->tetrominos);
	return (0);
}
