##
## EPITECH PROJECT, 2025
## r-type-mirror
## File description:
## Makefile
##

CC		=	g++

CFLAGS	=	-Wall -Wextra -I./include
NAME	=	r-type-server

SRC		=	$(shell find ./src -name "*.cpp")

all:
	$(CC) $(SRC) $(CFLAGS) -o $(NAME)
	cp $(NAME) ../

clean:
	rm -f *~

fclean:	clean
	rm -f $(NAME)
	rm -f ../$(NAME)

re:	fclean all
