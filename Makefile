CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL -lm
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h $(SOURCE)/shinage_math.h
	$(CC) $(CFLAGS) $(SOURCE)/shinage.c $(LIBS) -o shinage
