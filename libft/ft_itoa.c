/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 09:34:39 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 16:48:33 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static long	ft_calclen(long n)
{
	long	ten;

	ten = 1;
	while (n >= 10)
	{
		n /= 10;
		ten *= 10;
	}
	return (ten);
}

char	*ft_itoa(int n)
{
	int					i;
	int					ten;
	char				*nb;
	long				nbr;

	i = 0;
	nb = ft_calloc(13, sizeof(char));
	if (nb == NULL)
		return (NULL);
	if (n < 0)
	{
		nbr = -((long)n);
		nb[i++] = '-';
	}
	else
		nbr = n;
	ten = ft_calclen(nbr);
	while (ten >= 1)
	{
		nb[i++] = (nbr / ten) + '0';
		nbr -= ((nbr / ten) * ten);
		ten /= 10;
	}
	return (nb);
}
