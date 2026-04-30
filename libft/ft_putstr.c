/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini.42nice.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/08 06:02:39 by dzuntini          #+#    #+#             */
/*   Updated: 2023/06/12 12:45:27 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putstr(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		ft_putchar(str[i++]);
}


void ft_putnstr(char *str)
{
	ft_putstr(str);
	ft_putchar('\n');
}

void ft_puttwostr(char *s1, char *s2)
{
	ft_putstr(s1);
	ft_putstr(s2);
}