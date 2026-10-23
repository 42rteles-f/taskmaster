NAME = liblogger.a
CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c99
CPPFLAGS = -Iinclude
AR = ar
ARFLAGS = rcs

SRC = src/logger.c
OBJ = $(SRC:.c=.o)
TEST = test_logger

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

src/%.o: src/%.c include/logger.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST): tests/test_logger.c $(NAME)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(NAME) -o $@

test: $(TEST)
	./$(TEST)

clean:
	rm -f $(OBJ) $(TEST) test-taskmaster.log

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all test clean fclean re
