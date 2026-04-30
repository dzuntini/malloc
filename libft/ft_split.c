/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpillet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/20 17:28:55 by jpillet           #+#    #+#             */
/*   Updated: 2021/10/28 19:43:10 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	ft_nb_w(char *str, char c)
{
	int		i;
	int		w;

	i = 0;
	w = 0;
	while (str[i])
	{
		while (str[i] && str[i] == c)
			i++;
		if (str[i] != '\0')
			w++;
		while (str[i] && (str[i] != c))
			i++;
	}
	return (w);
}

static char	*ft_add_w(char **str, char c)
{
	int		l;
	char	*word;
	int		i;

	l = 0;
	while (str[0][l] && (str[0][l] != c))
		l++;
	word = malloc((l + 1) * sizeof(char));
	if (!(word))
		return (0);
	i = 0;
	while (str[0][0] && (str[0][0] != c))
	{
		word[i] = str[0][0];
		i++;
		(str[0])++;
	}
	word[i] = '\0';
	return (word);
}

char	**ft_split(char const *s, char c)
{
	char	**tw;
	int		nbw;
	int		cw;

	nbw = ft_nb_w((char *)s, c);
	tw = malloc((nbw + 1) * sizeof(char *));
	if (!(tw))
		return (0);
	cw = 0;
	while (cw < nbw)
	{
		while (s[0] && s[0] == c)
			s++;
		tw[cw] = ft_add_w((char **)&s, c);
		if (!(tw[cw]))
			return (0);
		cw++;
	}
	tw[nbw] = 0;
	return (tw);
}
