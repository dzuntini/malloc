# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dzuntini <dzuntini@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/25 13:56:27 by pcarella              #+#    #+#              #
#    Updated: 2021/10/11 17:13:50 by dzuntini         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Check and set HOSTTYPE if empty or undefined
ifeq ($(HOSTTYPE),)
HOSTTYPE 	:= $(shell uname -m)_$(shell uname -s)
endif

NAME		=	libft_malloc_$(HOSTTYPE).so
SONAME		=	libft_malloc.so

NO_COLOR	= \033[1;36m
OK_COLOR	= \033[0;32m
KILL_COLOR	= \033[0;31m
WARN_COLOR	= \033[0;33m

CFLAGS		=	-Wall -Werror -Wextra -I. -fPIC -pthread

SRCS	=	malloc.c	\
			free.c		\
			calloc.c	\
			realloc.c	\
			utils.c

OBJS =		$(SRCS:.c=.o)

TEST_SRCS	=	test.c
TEST_OBJS	=	$(TEST_SRCS:.c=.o)
TEST_NAME	=	test_malloc

all:		libft $(NAME)
			@ln -sf $(NAME) $(SONAME)
			@echo "${OK_COLOR}Created symbolic link: $(SONAME) -> $(NAME)${NO_COLOR}"

$(NAME):	$(OBJS)
			@make -C ./libft
			@$(CC) -shared $(CFLAGS) $(OBJS) ./libft/libft.a -o $(NAME)
			@echo "${OK_COLOR}Built $(NAME)${NO_COLOR}"
			@echo "${KILL_COLOR}I'll call this one Salloc. Malloc is memory Allocation , This is Shitty Allocation${NO_COLOR}"

test:		$(NAME)
			@LD_PRELOAD=./$(SONAME) /bin/ls -la
			@echo "${OK_COLOR}Test complete - your malloc was used!${NO_COLOR}"

exec:		libft $(TEST_NAME)
			@./$(TEST_NAME)

$(TEST_NAME):	$(TEST_OBJS) $(OBJS)
			@$(CC) $(CFLAGS) $(TEST_OBJS) $(OBJS) ./libft/libft.a -o $(TEST_NAME)
			@echo "${OK_COLOR}Built test executable: $(TEST_NAME)${NO_COLOR}"

libft:
			@make -C ./libft

clean:
			@make clean -C ./libft
			@rm -f $(OBJS) $(TEST_OBJS)
			@echo "${WARN_COLOR}Cleaned object files${NO_COLOR}"

fclean:		clean
			@rm -f $(NAME) $(SONAME) $(TEST_NAME)
			@make fclean -C ./libft
			@echo "${KILL_COLOR}Removed $(NAME), $(SONAME) and $(TEST_NAME)${NO_COLOR}"

re: 		fclean all

.PHONY:		re clean fclean all libft test exec
