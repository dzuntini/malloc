/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini.42nice.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 16:09:42 by dzuntini          #+#    #+#             */
/*   Updated: 2023/06/12 14:32:53 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, char *src, size_t dstsize)
{
	unsigned int	idst;
	unsigned int	lsrc;

	idst = 0;
	while (dst[idst] != '\0')
		idst++;
	lsrc = 0;
	while (lsrc + idst + 1 < dstsize && src[lsrc] != '\0')
	{
		dst[idst + lsrc] = src[lsrc];
		lsrc++;
	}
	dst[idst + lsrc] = '\0';
	lsrc = 0;
	while (src[lsrc] != '\0')
		lsrc++;
	if (dstsize == 0)
		return (lsrc);
	if (idst < dstsize)
		return (idst + lsrc);
	else
		return (dstsize + lsrc);
}

size_t	ft_strcat(char *dst, char *src)
{
	return (ft_strlcat(dst, src, (ft_strlen(dst) + ft_strlen(src))));
}