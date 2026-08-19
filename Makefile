NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS_DIR = srcs
INCS_DIR = includes
SRCS = \
	$(SRCS_DIR)/main.c \
	$(SRCS_DIR)/cleanup/cleanup.c \
	$(SRCS_DIR)/cleanup/destroy_mutexes.c \
	$(SRCS_DIR)/log/log.c \
	$(SRCS_DIR)/monitor/completion.c \
	$(SRCS_DIR)/monitor/monitor.c \
	$(SRCS_DIR)/parse/parse_args.c \
	$(SRCS_DIR)/parse/parse_int.c \
	$(SRCS_DIR)/parse/validate_args.c \
	$(SRCS_DIR)/runtime/coder_actions.c \
	$(SRCS_DIR)/runtime/coder_routine.c \
	$(SRCS_DIR)/runtime/simulation.c \
	$(SRCS_DIR)/runtime/stop.c \
	$(SRCS_DIR)/scheduler/edf_scheduler.c \
	$(SRCS_DIR)/scheduler/fifo_scheduler.c \
	$(SRCS_DIR)/scheduler/heap.c \
	$(SRCS_DIR)/scheduler/heap_compare.c \
	$(SRCS_DIR)/sync/acquire_dongles.c \
	$(SRCS_DIR)/sync/dongle.c \
	$(SRCS_DIR)/sync/release_dongles.c \
	$(SRCS_DIR)/sync/request.c \
	$(SRCS_DIR)/time/sleep.c \
	$(SRCS_DIR)/time/time.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean fclean re compile_commands

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCS_DIR) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

compile_commands:
	@printf '[\n' > compile_commands.json
	@for src in $(SRCS); do \
		printf '  {"directory":"%s","command":"$(CC) $(CFLAGS) -I$(INCS_DIR) -c %s -o %s","file":"%s"},\n' \
			"$(CURDIR)" "$$src" "$${src%.c}.o" "$$src" \
			>> compile_commands.json; \
	done
	@sed -i '$$ s/,$$//' compile_commands.json
	@printf ']\n' >> compile_commands.json
