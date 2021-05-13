CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL -lm -lXfixes -lfreetype
INCLUDES=`pkg-config --cflags freetype2`
CC=gcc
SOURCE=source

shinage: $(SOURCE)/x11_shinage.c $(SOURCE)/x11_shinage.h $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_input.h $(SOURCE)/shinage_shaders.h $(SOURCE)/shinage_opengl_signatures.h $(SOURCE)/shinage_utils.h $(SOURCE)/x11_shinage_text.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCE)/x11_shinage.c $(LIBS) -o shinage

tests: $(SOURCE)/tests.c $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_stack_structures.h
	$(CC) $(CFLAGS) $(SOURCE)/tests.c $(LIBS) -o tests

etags: %.c %.h
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -M $(SOURCE)/x11_shinage.c | awk 'NR==1 { for (i=2;i<NF;++i) print $$i } NR>1 { for(i=1;i<NF;++i) print $$i }' | etags -L -

clean:
	@rm -f shinage tests && echo "Done"
