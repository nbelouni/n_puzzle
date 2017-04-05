SRC_PATH	=	src/
OBJ_PATH	=	obj/
INC_PATH	=	inc/

SRCS		=	$(shell ls $(SRC_PATH) | grep .cpp$$)
OBJS		=	$(patsubst %.cpp, $(OBJ_PATH)%.o,$(SRCS))

PLATFORM	:=	$(shell uname)

HEADER		=	-I./$(INC_PATH) -I./compiled_GLFW/ -I./compiled_SOIL/
FLAGS		=	-O3 -Wall -Wextra -Werror -std=gnu++11


ifeq "$(PLATFORM)" "Darwin" #MAC
GLFW		=	./compiled_GLFW/libglfw3_darwin.a
SOIL		=	./compiled_SOIL/libSOIL.a
LIBS		=	$(SOIL) $(GLFW) -framework Carbon -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo
CC			=	g++
else ifeq "$(PLATFORM)" "Linux" #LINUX
GLFW		=	./compiled_GLFW/libglfw3_linux.a
SOIL		=	./compiled_SOIL/libSOIL.a
LIBS		=	$(SOIL) $(GLFW) -lGL -lXrandr -lXi -lXrender -ldrm -lXdamage -lXxf86vm -lXext -lX11 -lpthread -lXcursor -lm -lXinerama `libpng-config --libs` -ldl
CC			=	clang++
endif

NAME		=	n-puzzle

all: $(NAME)
$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(HEADER) $(OBJS) $(LIBS) -o $(NAME)

$(patsubst %, $(OBJ_PATH)%,%.o): $(SRC_PATH)$(notdir %.cpp)
	@mkdir -p $(OBJ_PATH)
	@$(CC) -c $(FLAGS) $(HEADER) "$<" -o "$@"

clean:
	@rm -rf $(OBJ_PATH)

fclean: clean
	@rm -f $(NAME)

re: fclean all

rl: re
	@./$(NAME)

ml: all
	@./$(NAME)

.PHONY: clean fclean re
