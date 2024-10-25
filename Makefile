SRCSCPP = main.cpp ./srcs/resHeader.cpp

OBJSCPP = $(patsubst %.cpp, objects/%.o, $(SRCSCPP))

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

# Add include directories manually (-I<path_to_headers>)
# Example: -I./include or -I./headers if you have folders with headers
# Here we are adding current directory and a hypothetical "includes" folder
CPPFLAGS = -I. -I./include -MMD -MP

NAME = webserv

$(NAME): $(OBJSCPP)
	c++ $(CFLAGS) -o $(NAME) $(OBJSCPP)

all: $(NAME)

objects/%.o: %.cpp
	mkdir -p $(dir $@)
	c++ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include automatically generated dependency files
-include $(OBJSCPP:.o=.d)

clean:
	rm -rf objects

fclean: clean
	rm -f $(NAME)

re: fclean all

ozan:
	make -f Makefile.ozan

.PHONY: all clean fclean re

