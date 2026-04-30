/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 12:15:32 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 16:16:32 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	unsigned int	i;
	char			*str;

	if (s)
	{
		i = 0;
		str = ft_calloc(len + 1, sizeof(char));
		if (str == NULL)
			return (NULL);
		if (start < len)
		{
			while (i < len)
			{
				str[i] = s[i + start];
				i++;
			}
		}
		return (str);
	}
	return (NULL);
}
