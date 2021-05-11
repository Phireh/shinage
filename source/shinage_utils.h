#ifndef SHINAGE_UTILS_H
#define SHINAGE_UTILS_H

#include "shinage_debug.h"

char* load_file(char *pathname)
{
	FILE *text_file = fopen(pathname, "r");
	if (!text_file)
		log_err("ERROR: Fail trying to read file at %s", pathname);
    fseek(text_file, 0, SEEK_END);
    long file_size = ftell(text_file);
    fseek(text_file, 0, SEEK_SET);  /* same as rewind(f); */

    char *file_contents = calloc(sizeof(char), file_size + 1);
    fread(file_contents, 1, file_size, text_file);
    fclose (text_file);
    return file_contents;
}

#endif