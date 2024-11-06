.DEFAULT_GOAL := all

# Colors and Styles
RED			:= \033[0;31m
GREEN		:= \033[0;32m
YELLOW		:= \033[0;33m
BLUE		:= \033[0;34m
MAGENTA		:= \033[0;35m
CYAN		:= \033[0;36m
RESET		:= \033[0m
BOLD		:= \033[1m
UNDERLINE	:= \033[4m

# Executable name
NAME		:=	webserv
# NAME1 = config

# Source directories and files
SRCS_DIR	:=	srcs
SRCS		=	srcs/ConfigParser.cpp \
				srcs/Location.cpp \
				srcs/main.cpp \
				srcs/Response.cpp \
				srcs/Utils.cpp \
				srcs/Server.cpp \
				srcs/ServerParser.cpp \
				srcs/Error.cpp \
				srcs/Post.cpp \
				srcs/Get.cpp
# srcs/Client.cpp
# srcs/socket.cpp

# SRCSPARSING =
FILE_TO_DELETE	:=	www/assets/files/file_to_delete

# Object directories and files
OBJS_DIR	:=	.objs
OBJS		:=	$(patsubst $(SRCS_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(SRCS))
# OBJSPARSING = $(patsubst %.cpp, objects/%.o, $(SRCSPARSING))

# Dependency directory
DEPS_DIR	:=	.deps
DEPS		:=	$(patsubst $(OBJS_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

# Compiler and flags
CXXFLAGS	:=	-Werror -Wall -Wextra -std=c++98 -g
CXX			:=	c++ $(CXXFLAGS)
DEPFLAGS	:=	-MMD -MP -MF
INCFLAGS	:=	-Iincludes
RM			:=	rm -rf

# Include automatically generated dependency files
-include $(DEPS)
# -include $(OBJSPARSING:.o=.d)

# Build the executable
$(NAME): $(OBJS)
	@$(CXX) -o $(NAME) $(OBJS)

# Compile source files into object files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@) $(DEPS_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(INCFLAGS) -c $< -o $@ $(DEPFLAGS) $(DEPS_DIR)/$*.d

# $(NAME1): $(OBJSPARSING)
# 	c++ $(CFLAGS) -o $(NAME1) $(OBJSPARSING)

all: $(NAME)

simple:
	@make all -s
	@clear
	@./webserv conf/Bastien.conf

# parsing : $(NAME1)

file_to_delete:
	@touch $(FILE_TO_DELETE)

# Display help information
help:
	@echo "$(BLUE)Makefile commands:$(RESET)"
	@echo "  make				Build the project (default target)"
	@echo "  make all			Build the project"
	@echo "  make clean			Remove object files and dependency files"
	@echo "  make fclean			Remove all files generated by the build"
	@echo "  make re			Clean and rebuild the project"
	@echo "  make help			Display this help message"
	@echo "  make info			Show information about the project"

# Display project information
info:
	@echo "$(GREEN)Project: $(NAME)$(RESET)"
	@echo "Source directory: $(SRCS_DIR)"
	@echo "Object directory: $(OBJS_DIR)"
	@echo "Dependency directory: $(DEPS_DIR)"
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"

# Check for issues
check:
	@echo "$(YELLOW)Checking for syntax errors...$(RESET)"
	@$(CXX) -fsyntax-only $(SRCS)
	@echo "$(YELLOW)Running static analysis with cppcheck...$(RESET)"
	@cppcheck --enable=all $(SRCS_DIR)
	@echo "$(YELLOW)Running static analysis with clang-tidy...$(RESET)"
	@clang-tidy $(SRCS) -- -I./includes
	@echo "$(YELLOW)Checking for coding style with clang-format...$(RESET)"
	@clang-format -style=Google -output-replacements-xml $(SRCS) | grep -c "<replacement"

# Clean object and dependency files
clean:
	@echo "$(RED)Cleaning up object files...$(RESET)"
	@$(RM) $(OBJ)
	@$(RM) $(OBJS_DIR)
	@$(RM) $(DEPS_DIR)

# Clean executable
fclean:	clean
	@echo "$(RED)Cleaning up $(NAME)...$(RESET)"
	@$(RM) $(NAME)
	@$(RM) $(FILE_TO_DELETE)

re: fclean all

# Phony targets
.PHONY: all clean fclean re

