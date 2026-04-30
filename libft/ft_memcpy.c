/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:46:12 by dzuntini          #+#    #+#             */
/*   Updated: 2021/01/23 10:36:15 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*t1;
	unsigned char	*t2;

	if ((!(dst)) && (!(src)))
		return (0);
	t1 = (unsigned char *)src;
	t2 = (unsigned char *)dst;
	while (n-- > 0)
		*t2++ = *t1++;
	return (dst);
}
