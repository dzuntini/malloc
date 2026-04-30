/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 11:54:33 by dzuntini          #+#    #+#             */
/*   Updated: 2021/07/06 16:41:00 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	ft_ischarset(char c, char const *cs)
{
	int	i;

	i = 0;
	while (cs[i])
	{
		if (cs[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static int	ft_checkcases(char const *s1, char const *s2)
{
	int	i;
	int	c;
	int	l;

	i = 0;
	c = 0;
	l = ft_strlen(s1);
	while (s1[i])
	{
		if (ft_ischarset(s1[i], s2))
			c++;
		i++;
	}
	if (c == 0)
		return (1);
	if (c == l)
		return (2);
	return (0);
}

static	char	*ft_strempty(void)
{
	char	*str;

	str = malloc(1 * sizeof(char));
	if (!(str))
		return (NULL);
	str[0] = '\0';
	return (str);
}

char	*ft_strtrim(char const *s1, char const *s2)
{
	int		l;
	size_t	i;
	char	*str;

	if (!s2 || ft_checkcases(s1, s2) == 1)
		return ((char *)s1);
	if (ft_checkcases(s1, s2) == 2)
		return (ft_strempty());
	if (s1)
	{
		i = 0;
		while (ft_ischarset(s1[i], s2))
			i++;
		l = ft_strlen(s1) - 1;
		while (ft_ischarset(s1[l], s2))
			l--;
		l++;
		str = ft_substr((char *)s1, i, l - i);
		if (str)
			return (str);
		else
			return (NULL);
	}
	return (NULL);
}
