/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 15:52:56 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 16:04:41 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*memalloc;

	memalloc = malloc(count * size);
	if (memalloc == NULL)
		return (NULL);
	ft_bzero(memalloc, (count * size));
	return (memalloc);
}
