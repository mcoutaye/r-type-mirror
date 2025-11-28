##
## EPITECH PROJECT, 2025
## r-type-mirror
## File description:
## Makefile
##

CC		= 	g++

CFLAGS	=	-W -Wall -Wextra -I./include -std=c++

NAME 	=	r-type

SRC 	= 	$(find ./src -name '*.cpp')

all:
	mkdir -p ./build
	@cd ./build && cmake .. && make
	cp ./build/$(NAME) .
	make -C ./server

clean:
	rm -rf ./build
	make clean -C ./server

fclean:	clean
	rm -f $(NAME)
	make fclean -C ./server

re:	fclean all
