CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL -lm -lXfixes -lfreetype -ldl
INCLUDES=-I./include
INCLUDES+=`pkg-config --cflags freetype2`
CC=gcc
TAGS_FLAVOR ?= etags
SOURCE=source
COMMON_SOURCES=$(SOURCE)/shinage_common.h $(SOURCE)/shinage_debug.h $(SOURCE)/shinage_math.h $(SOURCE)/shinage_matrix_stack_ops.h $(SOURCE)/shinage_input.h $(SOURCE)/shinage_opengl_signatures.h $(SOURCE)/shinage_shaders.h $(SOURCE)/shinage_scene.h $(SOURCE)/shinage_utils.h $(SOURCE)/shinage_ints.h
PLATFORM_SOURCES=$(SOURCE)/x11_shinage.c $(SOURCE)/x11_shinage.h $(COMMON_SOURCES)
GAME_SOURCES=$(SOURCE)/shinage_game.c $(COMMON_SOURCES)

all: shinage shinage_game.so

shinage: $(PLATFORM_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCE)/x11_shinage.c $(LIBS) -o shinage

shinage_game.so: $(GAME_SOURCES)
	$(CC) $(CFLAGS) -fPIC -shared $(INCLUDES) $(SOURCE)/shinage_game.c $(LIBS) -o shinage_game.so

tests: $(SOURCE)/tests.c $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_stack_structures.h
	$(CC) $(CFLAGS) $(SOURCE)/tests.c $(INCLUDES) $(LIBS) -o tests

.PHONY: tags gtags

tags: $(SOURCE)/*.c $(SOURCE)/*.h
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -M $(SOURCE)/x11_shinage.c | awk 'NR==1 { for (i=2;i<NF;++i) print $$i } NR>1 { for(i=1;i<NF;++i) print $$i }' | $(TAGS_FLAVOR) -L -

gtags: $(SOURCE)/*.c $(SOURCE)/*.h
	rm -rf gtags
	rm -rf GPATH GRTAGS GTAGS	
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -M $(SOURCE)/x11_shinage.c | awk 'NR==1 { for (i=2;i<NF;++i) print $$i } NR>1 { for(i=1;i<NF;++i) print $$i }' > filelist
	./gtags.sh filelist
	rm filelist
	gtags -w -v

clean:
	@rm -f shinage tests && echo "Done"
