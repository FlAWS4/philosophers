NAME = philo
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g

SRC_DIR = ./srcs/
SRC_FILES = main.c \
		philosophers.c \
		utils.c \
		init.c \
		monitor.c \
		action.c 
SRCS = $(addprefix $(SRC_DIR), $(SRC_FILES))

INCLUDES = -I./includes

OBJS_DIR = objs/
OBJS = $(addprefix $(OBJS_DIR), $(SRC_FILES:.c=.o))

RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re