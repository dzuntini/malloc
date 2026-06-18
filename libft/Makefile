#COMPILATION

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror
INC			= -I.

#VALUES
NAME		= libft.a

#MISC

NO_COLOR	= \033[1;36m	
OK_COLOR	= \033[0;32m
KILL_COLOR	= \033[0;31m
WARN_COLOR	= \033[0;33m

#SOURCES

FILES		= 	ft_atoi.c  			\
				ft_itoa.c			\
				ft_bzero.c 			\
				ft_calloc.c			\
				ft_freetab.c		\
				ft_isalnum.c		\
				ft_isalpha.c		\
				ft_isascii.c		\
				ft_isdigit.c		\
				ft_isprint.c		\
				ft_isxx2.c 			\
				ft_malloc.c			\
				ft_memccpy.c		\
				ft_memchr.c			\
				ft_memcmp.c			\
				ft_memcpy.c			\
				ft_memmove.c		\
				ft_memset.c			\
				ft_putchar.c		\
				ft_putnbr.c			\
				ft_putstr.c			\
				ft_strchr.c			\
				ft_strrchr.c		\
				ft_strlcat.c		\
				ft_strlcpy.c		\
				ft_strlen.c			\
				ft_strncmp.c		\
				ft_strnstr.c		\
				ft_strdup.c			\
				ft_tolower.c		\
				ft_toupper.c		\
				ft_strjoin.c		\
				ft_putchar_fd.c		\
				ft_putstr_fd.c		\
				ft_putnbr_fd.c		\
				ft_strtrim.c		\
				ft_substr.c			\
				ft_stris.c			\
				ft_strmapi.c		\
				ft_split.c			\
				ft_putendl_fd.c		\
				ft_lstdelone.c		\
				ft_lstsize.c		\
				ft_lstlast.c		\
				ft_lstclear.c		\
				ft_lstiter.c		\
				ft_lstnew.c			\
				ft_lstadd_back.c	\
				ft_lstadd_front.c	\
				ft_lstmap.c			\
				ft_printf.c			\
				ft_printerror.c		\
				get_next_line.c		\


OBJS		= ${FILES:.c=.o}

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

$(NAME):	${OBJS}
			ar -cr $(NAME) $(OBJS) libft.h 
			@echo "${OK_COLOR}lib created and ready"



all:	${NAME}

clean:
			@rm -f *.o; rm -f *~
			@echo "${WARN_COLOR}temporary files removed"

fclean: clean
				@rm -f $(NAME)
				@echo "${KILL_COLOR}lib deleted"
				@echo "${NO_COLOR}"

re:		fclean all

.PHONY: all clean fclean re
