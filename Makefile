CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL -lm
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_input.h
	$(CC) $(CFLAGS) $(SOURCE)/shinage.c $(LIBS) -o shinage

tests: $(SOURCE)/tests.c $(SOURCE)/shinage_math.h
	$(CC) $(CFLAGS) $(SOURCE)/tests.c $(LIBS) -o tests
