/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini.42nice.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 09:46:50 by dzuntini          #+#    #+#             */
/*   Updated: 2023/06/12 14:32:40 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <unistd.h>
# include <limits.h>
# include <stdlib.h>
# include <errno.h>
# include <stdio.h>
# include "get_next_line.h"


typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}				t_list;

void			ft_freetab(char **av);
void			ft_putchar(char c);
void			ft_putnbr(int nb);
void			ft_putstr(char *str);
void			ft_putnstr(char *str);
void			ft_puttwostr(char *s1, char *s2);
void			ft_putchar_fd(char c, int fd);
void			ft_putnbr_fd(int nb, int fd);
void			ft_putstr_fd(char *s, int fd);
void			ft_putendl_fd(char *s, int fd);
void			ft_bzero(void *s, size_t n);
void			ft_lstdelone(t_list *alst, void (*del)(void *));
void			ft_lstclear(t_list **lst, void (*del)(void *));
void			ft_lstiter(t_list *lst, void (*f)(void *));
void			ft_lstadd_back(t_list **alst, t_list *new);
void			ft_lstadd_front(t_list **alst, t_list *new);

char			**ft_split(char const *s, char c);

void			*ft_malloc(int size);
void			*ft_memset(void *b, int c, size_t len);
void			*ft_memcpy(void *dst, const void *src, size_t n);
void			*ft_memccpy(void *dst, const void *src, int c, size_t n);
void			*ft_memmove(void *dst, const void *src, size_t len);
void			*ft_memchr(const void *s, int c, size_t n);
void			*ft_calloc(size_t count, size_t size);

size_t			ft_strlen(const char *s);
size_t			ft_strlcat(char *dst, char *src, size_t dstsize);
size_t			ft_strcat(char *dst, char *src);
size_t			ft_strlcpy(char *dst, char *src, size_t dstsize);

int				ft_strncmp(const char *s1, const char *s2, size_t n);
int				get_next_line(int fd, char **line);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
int				ft_atoi(const char *str);
int				ft_isalpha(int c);
int				ft_isdigit(int c);
int				ft_isprint(int c);
int				ft_isascii(int c);
int				ft_isalnum(int c);
int				ft_isupper(int c);
int				ft_islower(int c);
int				ft_stralnum(char *str);
int				ft_strchar(char *str);
int				ft_strnbr(char *str);
int				ft_toupper(int c);
int				ft_tolower(int c);
int				ft_lstsize(t_list *list);

t_list			*ft_lstlast(t_list *lst);
t_list			*ft_lstnew(void *content);
t_list			*ft_lstmap(t_list *lst, void*(*f)(void *), void(*del)(void *));

char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
char			*ft_itoa(int n);
char			*ft_strdup(const char *s1);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
char			*ft_strnstr(const char *s, const char *n, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strtrim(char const *s1, char const *s2);
char			*ft_substr(char const *s, unsigned int start, size_t len);

void			ft_errorPut(char *str);
void			ft_errorPutExit(char *str);
void			ft_Perror(char *str, char *pref);
void			ft_PerrorFd(char *str, int fd, char *pref);

#endif
