SRCSCPP = main.cpp srcs/resHeader.cpp
SRCSPARSING = srcs/httpConfig.cpp srcs/locationConfig.cpp \
				srcs/serverConfig.cpp srcs/main.cpp srcs/resHeader.cpp \
				srcs/serverAddr.cpp

OBJSCPP = $(patsubst %.cpp, objects/%.o, $(SRCSCPP))
OBJSPARSING = $(patsubst %.cpp, objects/%.o, $(SRCSPARSING))

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

# Add include directories manually (-I<path_to_headers>)
# Example: -I./include or -I./headers if you have folders with headers
CPPFLAGS = -I. -I./include -MMD -MP

NAME = webserv
NAME1 = config

$(NAME): $(OBJSCPP)
	c++ $(CFLAGS) -o $(NAME) $(OBJSCPP)

$(NAME1): $(OBJSPARSING)
	c++ $(CFLAGS) -o $(NAME1) $(OBJSPARSING)

all: $(NAME)

parsing : $(NAME1)

objects/%.o: %.cpp
	mkdir -p $(dir $@)
	c++ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include automatically generated dependency files
-include $(OBJSCPP:.o=.d)
-include $(OBJSPARSING:.o=.d)

clean:
	rm -rf objects

fclean: clean
	rm -f $(NAME) $(NAME1)

re: fclean all

.PHONY: all clean fclean re parsing

