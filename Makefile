
## COLORS ##
END = \033[0m
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
WHITE = \033[1;37m

#COMPILATION
NAME = ircserv
CC = c++ -I./inc
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

OBJ_DIR = .obj
SRC_DIR = ./src

SRC = $(shell find $(SRC_DIR) -type f -name '*.cpp')

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))


#REGLAS
all: $(NAME)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@  $<

$(NAME): $(OBJ)
	@echo "$(NAME): $(BLUE)Objects compiled succesdfully$(END)"
	@echo "$(NAME): $(BLUE)Linking...$(END)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "$(NAME): $(BLUE) $(NAME) created !$(END)"

#REMEMBER! MLX rule for compiling mlx on linux.

clean:
	@rm -rf $(OBJ)
	@rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean all re