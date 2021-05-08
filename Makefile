CFLAGS=-Wall -Wextra -Werror -g
LIBS=-lX11 -lGL -lm -lXfixes
CC=gcc
SOURCE=source

shinage: $(SOURCE)/shinage.c $(SOURCE)/shinage.h $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_input.h $(SOURCE)/shinage_shaders.h $(SOURCE)/shinage_opengl_signatures.h $(SOURCE)/shinage_text.h
	$(CC) $(CFLAGS) $(SOURCE)/shinage.c $(LIBS) -o shinage

tests: $(SOURCE)/tests.c $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h
	$(CC) $(CFLAGS) $(SOURCE)/tests.c $(LIBS) -o tests

etags: $(SOURCE)/shinage.c $(SOURCE)/shinage.h $(SOURCE)/shinage_math.h $(SOURCE)/shinage_camera.h $(SOURCE)/shinage_input.h $(SOURCE)/shinage_shaders.h $(SOURCE)/shinage_opengl_signatures.h
	@$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -M $(SOURCE)/shinage.c | awk 'NR==1 { for (i=2;i<NF;++i) print $$i } NR>1 { for(i=1;i<NF;++i) print $$i }' | etags -L -
