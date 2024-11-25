.DEFAULT_GOAL := all

# # Colors and Styles
RESET		:=	\e[0m
BOLD		:=	\e[1m
DIM			:=	\e[2m
ITALIC		:=	\e[3m
UNDERLINE	:=	\e[4m
RED			:=	\e[31m
GREEN		:=	\e[32m
PURPLE		:=	\e[95m
BLUE		:=	\e[34m
YELLOW		:=	\e[33m
MAGENTA		:=	\e[35m
CYAN		:=	\e[36m

# Colored Messages
MESSAGE_CLEAR		=	\e[0K\r\c
MESSAGE_OK			=	[\e[32mOK\e[0m]
MESSAGE_COMPILE		=	$(BLUE)Compiling :$(RESET)
MESSAGE_DONE		=	$(MESSAGE_OK) WebServ compiled.
MESSAGE_CLEAN		=	$(PURPLE)WebServ cleanup completed.$(RESET)
MESSAGE_PASS		=	$(GREEN)Success: $$config failed as expected$(RESET)
MESSAGE_FAIL		=	$(RED)Failed: $$config should have failed but didn't$(RESET)
MESSAGE_TEST		=	---\n$(MAGENTA)Testing:

# Executable name
NAME		:=	webserv

# Source directories and files
SRCS_DIR	:=	srcs
SRCS		=	srcs/autoIndex.cpp \
				srcs/ConfigParser.cpp \
				srcs/Location.cpp \
				srcs/main.cpp \
				srcs/parsingUtils.cpp \
				srcs/Response.cpp \
				srcs/Utils.cpp \
				srcs/Server.cpp \
				srcs/CGIHandler.cpp \
				srcs/ServerParser.cpp \
				srcs/error.cpp \
				srcs/post.cpp \
				srcs/get.cpp

TEST_CONFS	=	conf/test/conflict_location.conf \
				conf/test/conflict_servername.conf \
				conf/test/empty.conf \
				conf/test/forbidden_directive.conf \
				conf/test/invalid_cgi_path.conf \
				conf/test/invalid_ip.conf \
				conf/test/invalid_port.conf \
				conf/test/empty_location.conf \
				conf/test/location_without_arg.conf \
				conf/test/no_http_context.conf \
				conf/test/no open_context.conf \
				conf/test/no_server.conf \
				conf/test/single_line.conf \
				conf/test/unclose_context.conf \
				conf/test/unclose_directive.conf \
				conf/test/do_not_exist.conf

FILE_TO_DELETE	:=	www/assets/files/file_to_delete

# Object directories and files
OBJS_DIR	:=	.objs
OBJS		:=	$(patsubst $(SRCS_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(SRCS))

# Dependency directory
DEPS_DIR	:=	.deps
DEPS		:=	$(patsubst $(OBJS_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

# Compiler and flags
CXXFLAGS	:=	-Werror -Wall -Wextra -std=c++98 -march=native
CXX			:=	c++ $(CXXFLAGS)
DEPFLAGS	:=	-MMD -MP -MF
INCFLAGS	:=	-Iincludes
RM			:=	rm -rf

# Include automatically generated dependency files
-include $(DEPS)

# Build the executable
$(NAME): $(OBJS)
	@$(CXX) -o $(NAME) $(OBJS)
	@echo "$(MESSAGE_DONE)"

# Compile source files into object files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@) $(DEPS_DIR)
	@echo "[...] $(MESSAGE_COMPILE) $<...\r\c"
	@$(CXX) $(INCFLAGS) -c $< -o $@ $(DEPFLAGS) $(DEPS_DIR)/$*.d
	@echo "$(MESSAGE_CLEAR)"

all: $(NAME)

simple:
	@clear
	@make -j4 -s
	@clear
	./webserv conf/test/config.conf

test:
	@clear
	@make -j4 -s
	@clear
	@echo "Testing WebServ configuration files..."
	@failure_count=0; \
	failed_confs=""; \
	for config in $(TEST_CONFS); do \
		echo "\nTesting $$config:"; \
		timeout 0.2s ./$(NAME) $$config; \
		if [ $$? -eq 0 ]; then \
			echo "$(MESSAGE_PASS)"; \
		else \
			echo "$(MESSAGE_FAIL)"; \
			failure_count=$$((failure_count + 1)); \
		fi; \
	done; \
	echo "\n$(YELLOW)Bad confs test completed.$(RESET)"; \
	if [ $$failure_count -eq 0 ]; then \
		echo "$(GREEN)All bad configurations were corectly handled.$(RESET)"; \
	else \
		echo "$(RED)$$failure_count bad configuration(s) were not handled.$(RESET)"; \
		echo "$(YELLOW)Please check the configurations below:$(RESET)"; \
		for config in $(TEST_CONFS); do \
			timeout 0.2s ./$(NAME) $$config > /dev/null 2>&1; \
			if [ $$? -ne 0 ]; then \
				echo "$(RED) - $$config$(RESET)"; \
			fi; \
		done; \
	fi

file_to_delete:
	@touch $(FILE_TO_DELETE)

# Display help information
help:
	@echo "$(BLUE)Makefile commands:$(RESET)"
	@echo "  make				Build the project (default target)"
	@echo "  make all			Build the project"
	@echo "  make clean			Remove object files and dependency files"
	@echo "  make fclean		\tRemove all files generated by the build"
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

