#ifndef SHINAGE_DEBUG_H
#define SHINAGE_DEBUG_H

#include <string.h>

/* Convenience macros */
// NOTE: ##__VA_ARGS__ is a compiler extension and may not be portable. Maybe check for compiler defs here.
#define log_err(str, ...) fprintf(stderr, "[ERROR] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(str, ...) fprintf(stderr, "[DEBUG] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(str, ...) fprintf(stderr, "[INFO] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

static inline void log_debug_matx4f(mat4x4f *m, char* txt)
{
  log_debug("%s\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n",
                  txt,
                  m->a1, m->b1, m->c1, m->d1,
                  m->a2, m->b2, m->c2, m->d2,
                  m->a3, m->b3, m->c3, m->d3,
                  m->a4, m->b4, m->c4, m->d4);
}

static inline void log_debug_vec4f(vec4f *vs, uint count, char* txt)
{
  const int MAX_BUF = 32 * count;
  char buffer[MAX_BUF];
  uint length = 0;
  for (uint i = 0; i < count; i++)
  {
    vec4f v = vs[i];
    length += snprintf(buffer+length, MAX_BUF-length,"%2.3f\t\t%2.3f\t\t%2.3f\t\t%2.3f\n", v.x, v.y, v.z, v.w);
  }
  log_debug("\n================================================================ \
    \n%s\n%s================================================================\n", txt, buffer);
}

static inline void log_debug_matx2f(mat2x2f *m, char* txt)
{
  log_debug("%s\n %.3f %.3f\n %.3f %.3f\n",
                  txt,
                  m->a1, m->b1,
                  m->a2, m->b2);
}

static inline void log_debug_matx3f(mat3x3f *m, char* txt)
{
  log_debug("%s\n %.3f %.3f %.3f\n %.3f %.3f %.3f\n %.3f %.3f %.3f\n",
                  txt,
                  m->a1, m->b1, m->c1,
                  m->a2, m->b2, m->c2,
                  m->a3, m->b3, m->c3);
}

#endif
