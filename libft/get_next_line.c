/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 08:45:05 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 18:50:42 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_strncpy(char *dest, char *src, unsigned int n)
{
	unsigned int	i;

	i = 0;
	if (!dest || !src)
		return (0);
	while (src[i] != '\0' && i < n)
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
		dest[i++] = '\0';
	dest[i] = '\0';
	return (dest);
}

char	*ft_add_to_line(char *line, int end, char *buff, int *start)
{
	char	*tmp;
	int		linelen;

	if (line != NULL)
		linelen = ft_strlen(line);
	else
		linelen = 0;
	tmp = ft_malloc((linelen + end + 1) * sizeof(char));
	if (line != 0)
		ft_strncpy(tmp, line, linelen);
	else
		ft_strncpy(tmp, "", linelen);
	tmp[linelen + end] = 0;
	ft_strncpy(tmp + linelen, buff + *start, end);
	if (line)
		free(line);
	*start = *start + (end + 1);
	return (tmp);
}

int	ft_get_next_line(int fd, t_struct *t)
{
	int				end;

	end = 0;
	t->line = 0;
	while (42)
	{
		if (t->nbinbuff <= t->start)
		{
			t->start = 0;
			t->nbinbuff = read(fd, t->buffer, BUFFER_SIZE);
			if (t->nbinbuff == -1 || t->nbinbuff == 0)
				return (t->nbinbuff);
			t->buffer[t->nbinbuff] = '\0';
			end = 0;
		}
		if (t->buffer[t->start + end] == '\n')
		{
			t->line = ft_add_to_line(t->line, end, t->buffer, &t->start);
			return (1);
		}
		if (t->start + end == t->nbinbuff - 1)
			t->line = ft_add_to_line(t->line, end + 1, t->buffer, &t->start);
		end = end + 1;
	}
}

int	get_next_line(int fd, char **line)
{
	int				status;
	static t_struct	t[255];

	if (fd > 255 || fd < 0 || !line || BUFFER_SIZE <= 0)
		return (-1);
	status = ft_get_next_line(fd, &t[fd]);
	*line = t[fd].line;
	if (*line == NULL)
	{
		*line = ft_malloc(1);
		ft_bzero(*line, 1);
	}
	return (status);
}
