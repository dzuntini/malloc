/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/01 12:56:53 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 18:50:52 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>
# include "libft.h"

# define BUFFER_SIZE 4096

typedef struct s_struct
{
	char	buffer[BUFFER_SIZE + 1];
	char	*line;
	int		start;
	int		nbinbuff;

}				t_struct;

int				ft_get_next_line(int fd, t_struct *t);
char			*ft_add_to_line(char *line, int end, char *buff, int *start);
int				get_next_line(int fd, char **line);
char			*ft_strncpy(char *dest, char *src, unsigned int n);
#endif
