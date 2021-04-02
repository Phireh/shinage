CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h $(SOURCE)/shinage_math.h
	$(CC) $(CFLAGS) $(LIBS) $(SOURCE)/shinage.c -o shinage
