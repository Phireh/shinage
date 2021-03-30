CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h
	$(CC) $(CFLAGS) $(LIBS) $(SOURCE)/shinage.c -o shinage
