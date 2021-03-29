CFLAGS=-Wall -Wextra -Werror
LIBS=-lX11
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h
	$(CC) $(CFLAGS) $(LIBS) $(SOURCE)/shinage.c -o shinage
