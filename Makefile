NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS_DIR = src
SRCS = main.c
OBJS = $($(SRCS_DIR)/SRCS:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

