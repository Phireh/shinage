#include "shinage.h"


int main(int argc, char *argv[])
{
  /* Command args handling */
  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
    {
      printf("Usage:\n");
      printf("\t-h | --help:\tPrint help\n");
      printf("\t-v | --version:\tPrint version number\n");
    }
    if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
    {
      printf("SHINAGE version %s", version);
      return 0;
    }
  }

  /* X11 Window creation */
  if (!(x11_display = XOpenDisplay(NULL)))
  {
    log_err("Could not open X11 display\n");
    return 0;
  }

  x11_screen = DefaultScreenOfDisplay(x11_display);
  x11_screen_id = DefaultScreen(x11_display);

  /* GLX initialization */
  GLint glx_attr[] = {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      None
  };

  int fb_candidate_configs_count = 0;
  GLXFBConfig *candidate_fbs = glXChooseFBConfig(x11_display, x11_screen_id, glx_attr, &fb_candidate_configs_count);
  if (!fb_candidate_configs_count)
  {
      log_err("Unable to find framebuffer candidates for desired config");
      return 1;
  }  
  log_info("Found %d framebuffer candidates", fb_candidate_configs_count);

  int best_fb_index = -1;
  int best_sample_count = 0;

  for (int i = 0; i < fb_candidate_configs_count; ++i)
  {
      XVisualInfo *candidate_info = glXGetVisualFromFBConfig(x11_display, candidate_fbs[i]);
      if (candidate_info)
      {
          int sample_buffers;
          int samples;
          glXGetFBConfigAttrib(x11_display, candidate_fbs[i], GLX_SAMPLE_BUFFERS, &sample_buffers);
          glXGetFBConfigAttrib(x11_display, candidate_fbs[i], GLX_SAMPLES       , &samples);

          if (best_fb_index < 0 || (sample_buffers && samples > best_sample_count))
          {
              best_fb_index = i;
              best_sample_count = samples;
          }          
      }
      XFree(candidate_info);
  }
  log_debug("Best visual info index is %d", best_fb_index);

  GLXFBConfig best_fb_candidate = candidate_fbs[best_fb_index];
  XFree(candidate_fbs);

  XVisualInfo *glx_visual = glXGetVisualFromFBConfig(x11_display, best_fb_candidate);
  if (!glx_visual)
  {
      log_err("Unable to create GLX virtual");
      return 1;
  }

  if (x11_screen_id != glx_visual->screen)
  {
      log_err("Screen ID %d does not match visual->screen %d", x11_screen_id, glx_visual->screen);
      return 1;
  }

  GLint major_glx_version, minor_glx_version;
  glXQueryVersion(x11_display, &major_glx_version, &minor_glx_version);
  log_info("GLX version %d.%d", major_glx_version, minor_glx_version);
  

  XSetWindowAttributes x11_window_set_attr;
  x11_window_set_attr.border_pixel = BlackPixel(x11_display, x11_screen_id);
  x11_window_set_attr.background_pixel = WhitePixel(x11_display, x11_screen_id);
  x11_window_set_attr.override_redirect = True;
  x11_window_set_attr.colormap = XCreateColormap(x11_display, RootWindow(x11_display, x11_screen_id), glx_visual->visual, AllocNone);
  x11_window_set_attr.event_mask = KeyPressMask | KeyReleaseMask | KeymapStateMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  

  // NOTE: We can't use XCreateSimpleWindow if we need GLX/OpenGL
  x11_window = XCreateWindow(x11_display,
                             RootWindowOfScreen(x11_screen),
                             x11_window_x_coord,
                             x11_window_y_coord,
                             x11_window_width,
                             x11_window_height,
                             x11_window_border_size,
                             glx_visual->depth,
                             InputOutput,
                             glx_visual->visual,
                             CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
                             &x11_window_set_attr);

  // Register WM_DELETE_WINDOW so we can handle the window manager's close request in our message dispatch                           
  Atom wmDeleteMessage = XInternAtom(x11_display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(x11_display, x11_window, &wmDeleteMessage, 1);

  /* Create GLX OpenGL context */

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

  int context_attr[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 2,
      GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
      None
  };

  if (check_for_glx_extension("GLX_ARB_create_context", x11_display, x11_screen_id))
  {
      log_info("glXCreateContextAttribsARB is supported");
      glx_context = glXCreateContextAttribsARB(x11_display, best_fb_candidate, 0, true, context_attr);
  }
  else
  {
      glx_context = glXCreateNewContext(x11_display, best_fb_candidate, GLX_RGBA_TYPE, 0, True);
      log_info("glXCreateContextAttribsARB is not supported");
  }
  XSync(x11_display, False);  
  glXMakeCurrent(x11_display, x11_window, glx_context);

  /* Get OpenGL function addresses and link manually since we do not use GLEW or similar libs
     Failing to do this will make us SIGSEGV on GL calls */
  link_gl_functions();
     
  
  /* Verify if context is direct */
  if (glXIsDirect(x11_display, glx_context))
  {
      log_info("GLX context is direct");
  }
  else
  {
      log_info("GLX context is indirect");
  }
  
  log_info("Starting OpenGL version %s", glGetString(GL_VERSION));

  // Set title name for our open window
  XStoreName(x11_display, x11_window, "shinage");
  
  XClearWindow(x11_display, x11_window);

  // Actually show the window
  XMapRaised(x11_display, x11_window);

  // How large is the window
  XWindowAttributes x11_window_attr;
  XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);

  /* Window message loop */
  XEvent x11_event;
  KeySym keysym = 0;
  main_loop_state_t main_loop_state = RUNNING;
  while (main_loop_state == RUNNING)
  {
      /* Event handling */      
      // TODO: Sleep so we don't burn the CPU
      XNextEvent(x11_display, &x11_event);

      switch (x11_event.type)
      {
      case KeymapNotify:
      case MappingNotify:
          // TODO: Maybe MappingNotify needs more logic?
          XRefreshKeyboardMapping(&x11_event.xmapping);
          break;

      case KeyPress:
          keysym = XLookupKeysym(&x11_event.xkey, 0);

          /* Look for modifier keys */
          unsigned int mod_keys = ((XKeyEvent*)&x11_event)->state;
          dispatch_mod_keys(mod_keys);
          log_debug("Pressed key %s%s", mod_key_str_prefix(), XKeysymToString(keysym));
          
          switch (keysym)
          {
          case XK_Escape:
              main_loop_state = FINISHED;
              break;

          case XK_Shift_L:
          case XK_Shift_R:
              set_shift_key();
              break;
          case XK_Control_L:
          case XK_Control_R:
              set_ctrl_key();
              break;
          case XK_Alt_L:
          case XK_Alt_R:
              set_alt_key();
              break;                            
          }
          break;

      case KeyRelease:

          keysym = XLookupKeysym(&x11_event.xkey, 0);
          switch (keysym)
          {
          case XK_Shift_L:
          case XK_Shift_R:
              unset_shift_key();
              break;
          case XK_Control_L:
          case XK_Control_R:
              unset_ctrl_key();
              break;
          case XK_Alt_L:
          case XK_Alt_R:
              unset_alt_key();
              break;                            
          }
          break;

      case ButtonPress:
        ; // HACK: C does not permit declarations after a case statement, here is an empty statement to make compiler happy
        
        // TODO: Handle mouse button presses
        // TODO: Handle mod keys
        unsigned int button_pressed = ((XButtonEvent*)&x11_event)->button;
        int x = ((XButtonEvent*)&x11_event)->x;
        int y = ((XButtonEvent*)&x11_event)->y;
        log_debug("Pressed mouse button %d at coords (%d,%d)", button_pressed, x, y);
        switch (button_pressed) {
        case Button1:
          break;
        case Button2:
          break;
        case Button3:
          break;
        case Button4:
          break;
        case Button5:
          break;                    
        }
          
          break;

      case ButtonRelease:
          // TODO: Handle mouse button releases
          break;

      case MotionNotify:
          // TODO: Handle mouse drags
          break;

      case ClientMessage:
          /* Petition to close window by the window manager. See Xlib's ICCCM docs */
          if (x11_event.xclient.data.l[0] == (long int)wmDeleteMessage)
          {
              main_loop_state = FINISHED;
              log_debug("Closing window by window manager's request");
          }
          break;

      case Expose:
          /* The window has been resized */
          XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);
          log_debug("Window resized; Window width %d; Window height %d", x11_window_attr.width, x11_window_attr.height);
          break;
      }

      /* Rendering */

      // NOTE: This is just for testing that OpenGL actually works
      glClearColor(1.0f, 0.6f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      draw_gl_triangle();      
      glXSwapBuffers(x11_display, x11_window);      
  }

  /* Cleanup */  
  XDestroyWindow(x11_display, x11_window);
  XCloseDisplay(x11_display);
  return 1;  
}

/* Check for the presence of an OpenGL extension using a name, like "GL_EXT_bgra".
   These strings need correct formatting (no spaces).
   Returns 1 on success, 0 on failure.
   Edited from https://www.opengl.org/archives/resources/features/OGLextensions/
*/
int check_for_glx_extension(char *extension, Display *display, int screen_id)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    /* Extension names should not have spaces. */
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;
    extensions = (const GLubyte *) glXQueryExtensionsString(display, screen_id);
//    log_debug("Extensions found: %s", extensions);
    /* It takes a bit of care to be fool-proof about parsing the
       OpenGL extensions string. Don't be fooled by sub-strings,
       etc. */
    start = extensions;
    for (;;) {
        where = (GLubyte *) strstr((const char *) start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;
        start = terminator;
    }
    return 0;
}


void draw_gl_triangle(void)
{
    float vertices[] = {
       -0.5f,  -0.5f,
        0.5f,  -0.5f,
        0.0f, 0.5f
        
    };
    
    static unsigned int triangle_program = 0;
    if (!triangle_program)
        triangle_program = make_gl_program(test_vertex_shader, test_fragment_shader);
    glUseProgram(triangle_program);

    static unsigned int vao;
    if (!vao)
        glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    static unsigned int vbo = 0;
    if (!vbo)
        glGenBuffers(1, &vbo);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


unsigned int make_gl_program(char *vertex_shader_source, char *fragment_shader_source)
{
    char infoLog[512];        
    unsigned int vertex_shader = build_shader(vertex_shader_source, GL_VERTEX_SHADER);
    unsigned int fragment_shader = build_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // print linking errors if any
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)         // TODO: Maybe better error handling?
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Error: shader linking failed: %s\n", infoLog);
    }
    else
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }    
    return program;
}


unsigned int build_shader(char *source, int type)
{
    char infoLog[512];
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar * const *)(&source), NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error: shader compilation failed: %s\n", infoLog);        
    }
    // TODO: Maybe better error handling?
    return shader;
}

int link_gl_functions(void)
{
    // TODO: Error handling
    glUseProgram              = (PFNGLUSEPROGRAMPROC)             glXGetProcAddress((const GLubyte *)"glUseProgram");
    glGetShaderiv             = (PFNGLGETSHADERIVPROC)            glXGetProcAddress((const GLubyte *)"glGetShaderiv");
    glShaderSource            = (PFNGLSHADERSOURCEPROC)           glXGetProcAddress((const GLubyte *)"glShaderSource");
    glCompileShader           = (PFNGLCOMPILESHADERPROC)          glXGetProcAddress((const GLubyte *)"glCompileShader");
    glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)       glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");
    glCreateShader            = (PFNGLCREATESHADERPROC)           glXGetProcAddress((const GLubyte *)"glCreateShader");
    glCreateProgram           = (PFNGLCREATEPROGRAMPROC)          glXGetProcAddress((const GLubyte *)"glCreateProgram");
    glDeleteShader            = (PFNGLDELETESHADERPROC)           glXGetProcAddress((const GLubyte *)"glDeleteShader");
    glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)           glXGetProcAddress((const GLubyte *)"glGetProgramiv");
    glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)      glXGetProcAddress((const GLubyte *)"glGetProgramInfoLog");
    glAttachShader            = (PFNGLATTACHSHADERPROC)           glXGetProcAddress((const GLubyte *)"glAttachShader");
    glLinkProgram             = (PFNGLLINKPROGRAMPROC)            glXGetProcAddress((const GLubyte *)"glLinkProgram");
    glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)        glXGetProcAddress((const GLubyte *)"glGenVertexArrays");
    glGenBuffers              = (PFNGLGENBUFFERSPROC)             glXGetProcAddress((const GLubyte *)"glGenBuffers");
    glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)        glXGetProcAddress((const GLubyte *)"glBindVertexArray");
    glBindBuffer              = (PFNGLBINDBUFFERPROC)             glXGetProcAddress((const GLubyte *)"glBindBuffer");    
    glBufferData              = (PFNGLBUFFERDATAPROC)             glXGetProcAddress((const GLubyte *)"glBufferData");
    glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)    glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");
    
    return 1;
}
