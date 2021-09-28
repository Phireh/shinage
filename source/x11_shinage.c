#include "x11_shinage.h"

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
    x11_window_set_attr.event_mask = KeyPressMask | KeyReleaseMask | KeymapStateMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask;


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

    // Start the FreeType library and load default font
    if (!init_freetype(&ft_library))
        log_debug("FreeType started");
    if (!load_face(ft_library, default_font_path, &default_face))
        log_debug("Default font face loaded");

    // Check if we loaded all characters we wanted
    int loaded_chars = load_charmap(default_face);
    if (loaded_chars == sizeof(charmap)/sizeof(charmap[0]))
        log_debug("Successfully loaded charmap");
    else
        log_debug("Failed to load complete charmap. %d chars loaded", loaded_chars);


    // Set title name for our open window
    XStoreName(x11_display, x11_window, "shinage");

    XClearWindow(x11_display, x11_window);

    // Actually show the window
    XMapRaised(x11_display, x11_window);

    Bool detectable_autorepeat_sup;
    XkbSetDetectableAutoRepeat(x11_display, true, &detectable_autorepeat_sup);
    log_debug("Detectable auto rep %d", detectable_autorepeat_sup);

    // How large is the window
    XWindowAttributes x11_window_attr;
    XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);


    // Disable vsync
    if (!set_vsync(false))
    {
        log_debug("Error trying to set Vsync off");
    }



    /* Game state initialization */
    /* NOTE: Still on the process of moving all state inside game_state.
     For now we're probably end up using pointers to his members. */
    game_state_t game_state = {0};
    game_state.loop_state = RUNNING;
    game_state.window_width = x11_window_width;
    game_state.window_height = x11_window_height;
    game_state.vsync = true;
    game_state.curr_frame_input = curr_frame_input;
    game_state.last_frame_input = last_frame_input;


    // Shaders init
    build_programs(&game_state);

    update_global_vars(&game_state);
    build_matrices();
    set_mat(PROJECTION, &game_state);
    float ar = (float)x11_window_width / (float)x11_window_height;
    perspective_camera(M_PI / 2, ar, 0.1f, 100.0f);
    set_mat(VIEW, &game_state);
    vec3f eye = { .x = 0, .y = 3, .z = -3 };
    vec3f poi = { .x = 0, .y = 0, .z = 0 };
    look_at(eye, poi, up_vector);

    /* Game code initialization */
    game_code_t game_code = {0};
    void *library_handle = dlopen("./shinage_game.so", RTLD_NOW);
    void *address = dlsym(library_handle, "game_update");
    if (!address)
    {
        fprintf(stderr, "Error loading game code: %s\n", dlerror());
    }
    game_code.game_update = address;
    address = dlsym(library_handle, "game_render");
    if (!address)
    {
        fprintf(stderr, "Error loading game code: %s\n", dlerror());
    }
    game_code.game_render = address;

    /* Main loop */
    double curr_frame_start_time = get_current_time();
    double last_frame_start_time = 0.0;
    double sleep_time = 0.0;
    double real_time_last_frame = 0.0;

    XEvent x11_event;
    KeySym keysym = 0;

    game_state.loop_state = RUNNING;
    while (game_state.loop_state == RUNNING)
    {
        /* We only need to sleep if VSync is off and the curr frame is not the first one */
        if (!game_state.vsync && game_state.framecount)
        {
            /* TODO: Include controls for upping the framerate target so we can test time-independence of logic code */
            last_frame_start_time = curr_frame_start_time;
            curr_frame_start_time = get_current_time();
            dt = curr_frame_start_time - last_frame_start_time;

            real_time_last_frame = dt - sleep_time;
            sleep_time = target_s_per_frame - real_time_last_frame;

            //log_debug("Delta time %f , we should sleep for %f s", dt, sleep_time);


            if (sleep_time > 0.0) /* Only try to sleep if we actually need to */
            {
                const struct timespec req = {
                    .tv_sec = 0,
                    // TODO: Figure out if we need extra time
                    .tv_nsec = sleep_time * 1000 * 1000 * 1000 // we sleep for enough ns to burn our extra time
                };
                struct timespec rem;
                int nsleep_ret = nanosleep(&req, &rem);
                if (nsleep_ret)
                {
                    switch (errno)
                    {
                    case EFAULT:
                        log_debug("Error copying information from used space during nanosleep");
                        break;
                    case EINTR:
                        log_debug("Nanosleep interrupted by a signal");
                        break;
                    case EINVAL:
                        log_debug("Value of tv_nsec is invalid");
                        break;
                    default:
                        log_debug("Unknown error during nanosleep");
                        break;
                    }
                }
                else
                {
                    //log_debug("We slept for %f s", get_current_time() - curr_frame_start_time);
                }
            }
        }

        /* NOTE: The basic input handling loop is as follows:
           We mantain 2 different structures: the last frame, and current frame inputs.

           Each frame we copy the last recorded input, then use the function
           consume_first_presses to make the sure keys are only in JUST_RELEASED/PRESSED
           states for a single frame.
        */

        double input_phase_stamp = get_current_time();


        for (int i = 0; i < MAX_PLAYERS; ++i)
        {
            last_frame_input[i] = curr_frame_input[i];
        }

        player_input_t *player1_input = &curr_frame_input[PLAYER_1];
        player_input_t *player1_last_input = &last_frame_input[PLAYER_1];

        /* HACK: We are just starting a frame, so there can be no JUST_PRESSED
           buttons just yet. Clean all of them for all active players */
        consume_first_presses(player1_input);

        /* Event handling */
        while(XPending(x11_display))
        {
            XNextEvent(x11_display, &x11_event);

            switch (x11_event.type)
            {
            case KeymapNotify:
            case MappingNotify:
                // TODO: Maybe MappingNotify needs more logic?
                // TODO: Input remapping
                XRefreshKeyboardMapping(&x11_event.xmapping);
                break;

            case KeyPress:
                keysym = XLookupKeysym(&x11_event.xkey, 0);

                /* Look for modifier keys */
                unsigned int mod_keys = ((XKeyEvent*)&x11_event)->state;
                dispatch_mod_keys(mod_keys);

                switch (keysym)
                {
                case XK_Escape:
                    game_state.loop_state = FINISHED;
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

                case XK_W:
                case XK_w:
                    set_input_state(&player1_input->forward,
                                    &player1_last_input->forward,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_S:
                case XK_s:
                    set_input_state(&player1_input->back,
                                    &player1_last_input->back,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_A:
                case XK_a:
                    set_input_state(&player1_input->left,
                                    &player1_last_input->left,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_D:
                case XK_d:
                    set_input_state(&player1_input->right,
                                    &player1_last_input->right,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_R:
                case XK_r:
                    set_input_state(&player1_input->up,
                                    &player1_last_input->up,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_F:
                case XK_f:
                    set_input_state(&player1_input->down,
                                    &player1_last_input->down,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_space:
                    set_input_state(&player1_input->space,
                                    &player1_last_input->space,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_Q:
                case XK_q:
                    set_input_state(&player1_input->shoulder_left,
                                    &player1_last_input->shoulder_left,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_E:
                case XK_e:
                    set_input_state(&player1_input->shoulder_right,
                                    &player1_last_input->shoulder_right,
                                    PRESSED,
                                    input_phase_stamp);
                    break;

                case XK_F1:
                    set_input_state(&player1_input->f1,
                                    &player1_last_input->f1,
                                    PRESSED,
                                    input_phase_stamp);
                    break;
                case XK_F2:
                    set_input_state(&player1_input->f2,
                                    &player1_last_input->f2,
                                    PRESSED,
                                    input_phase_stamp);
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

                case XK_W:
                case XK_w:
                    set_input_state(&player1_input->forward,
                                    &player1_last_input->forward,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_S:
                case XK_s:
                    set_input_state(&player1_input->back,
                                    &player1_last_input->back,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_A:
                case XK_a:
                    set_input_state(&player1_input->left,
                                    &player1_last_input->left,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_D:
                case XK_d:
                    set_input_state(&player1_input->right,
                                    &player1_last_input->right,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_R:
                case XK_r:
                    set_input_state(&player1_input->up,
                                    &player1_last_input->up,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_F:
                case XK_f:
                    set_input_state(&player1_input->down,
                                    &player1_last_input->down,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_space:
                    set_input_state(&player1_input->space,
                                    &player1_last_input->space,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_Q:
                case XK_q:
                    set_input_state(&player1_input->shoulder_left,
                                    &player1_last_input->shoulder_left,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_E:
                case XK_e:
                    set_input_state(&player1_input->shoulder_right,
                                    &player1_last_input->shoulder_right,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;

                case XK_F1:
                    set_input_state(&player1_input->f1,
                                    &player1_last_input->f1,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                case XK_F2:
                    set_input_state(&player1_input->f2,
                                    &player1_last_input->f2,
                                    UNPRESSED,
                                    input_phase_stamp);
                    break;
                }
                break;

            case ButtonPress:
                {

                    unsigned int button_pressed = ((XButtonEvent*)&x11_event)->button;
                    //int x = ((XButtonEvent*)&x11_event)->x;
                    //int y = ((XButtonEvent*)&x11_event)->y;
                    switch (button_pressed) {
                    case LEFT_MOUSE_BUTTON:
                        set_input_state(&player1_input->mouse_left_click,
                                        &player1_last_input->mouse_left_click,
                                        PRESSED,
                                        input_phase_stamp);
                        break;
                    case WHEEL_PRESS_MOUSE_BUTTON:
                        break;
                    case RIGHT_MOUSE_BUTTON:
                        set_input_state(&player1_input->mouse_right_click,
                                        &player1_last_input->mouse_right_click,
                                        PRESSED,
                                        input_phase_stamp);
                        break;
                    case WHEEL_UP_MOUSE_BUTTON:
                        break;
                    case WHEEL_DOWN_MOUSE_BUTTON:
                        break;
                    default:
                        /* TODO: Despite not being defined in X11/X.h,
                           we can capture button presses from additional mouse buttons (8, 9...)
                        */
                        break;
                    }
                } break;

            case ButtonRelease:
                {
                    unsigned int button_pressed = ((XButtonEvent*)&x11_event)->button;
                    /*int x = ((XButtonEvent*)&x11_event)->x;
                      int y = ((XButtonEvent*)&x11_event)->y;
                      log_debug("Released mouse button %d at coords (%d,%d)", button_pressed, x, y);*/
                    switch (button_pressed)
                    {
                    case LEFT_MOUSE_BUTTON:
                        set_input_state(&player1_input->mouse_left_click,
                                        &player1_last_input->mouse_left_click,
                                        UNPRESSED,
                                        input_phase_stamp);
                        break;
                    case WHEEL_PRESS_MOUSE_BUTTON:
                        break;
                    case RIGHT_MOUSE_BUTTON:
                        set_input_state(&player1_input->mouse_right_click,
                                        &player1_last_input->mouse_right_click,
                                        UNPRESSED,
                                        input_phase_stamp);
                        break;
                    case WHEEL_UP_MOUSE_BUTTON:
                        break;
                    case WHEEL_DOWN_MOUSE_BUTTON:
                        break;
                    default:
                        /* TODO: Despite not being defined in X11/X.h,
                           we can capture button presses from additional mouse buttons (8, 9...)
                        */
                        break;
                    }
                } break;

            case MotionNotify:
                {
                    // TODO: Mouse input for other players
                    int x = ((XPointerMovedEvent*)&x11_event)->x;
                    int y = ((XPointerMovedEvent*)&x11_event)->y;

                    if (player1_input->pointer_state == NORMAL)
                    {
                        /* Save old cursor coords */
                        player1_last_input->cursor_x = player1_input->cursor_x;
                        player1_last_input->cursor_y = player1_input->cursor_y;

                        /* Store the new ones */
                        player1_input->cursor_x = x;
                        player1_input->cursor_y = y;
                    }
                    else if (player1_input->pointer_state == GRABBED)
                    {
                        /* The old coords will always be the center of the screen, since we're grabbed.
                           This frame's cursor coords will be changed to it too, after we're done with handling input.
                           Our only job here is to store the temp value before we move the cursor back.
                        */
                        player1_input->cursor_x = x;
                        player1_input->cursor_y = y;
                    }

                    //log_debug("Moved pointer to (%d,%d)", x, y);
                } break;

            case ClientMessage:
                /* Petition to close window by the window manager. See Xlib's ICCCM docs */
                if (x11_event.xclient.data.l[0] == (long int)wmDeleteMessage)
                {
                    game_state.loop_state = FINISHED;
                    //log_debug("Closing window by window manager's request");
                }
                break;

            case Expose:
                /* The window has been resized */
                XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);
                x11_window_width = x11_window_attr.width;
                x11_window_height = x11_window_attr.height;
                /* Update glVierport coordinates to properly adjust to the new window size */
                glViewport(0, 0, x11_window_width, x11_window_height);
                //log_debug("Window resized; Window width %d; Window height %d", x11_window_attr.width, x11_window_attr.height);
                break;
            }
        }

        /* Auxiliar struct members for comfier input handling */

        if (player1_input->pointer_state == NORMAL)
        {
            player1_input->cursor_x_delta = (int)player1_input->cursor_x - (int)player1_last_input->cursor_x;
            player1_input->cursor_y_delta = (int)player1_input->cursor_y - (int)player1_last_input->cursor_y;
        }
        else if (player1_input->pointer_state == GRABBED)
        {
            player1_input->cursor_x_delta = (int)player1_input->cursor_x - (int)x11_window_width/2;
            player1_input->cursor_y_delta = (int)player1_input->cursor_y - (int)x11_window_height/2;

            /* NOTE: Since we're grabbed, we return the cursor to the center of the window immediately after
               we calculated the cursor delta */
            player1_input->cursor_x = (int)x11_window_width/2;
            player1_input->cursor_y = (int)x11_window_height/2;

            if (XWarpPointer(x11_display, None, x11_window, 0, 0, 0, 0, (int)(x11_window_width/2), (int)(x11_window_height/2)) == BadWindow)
            {
                log_debug("Error trying to move pointer to center of window: BadWindow");
            }
        }


        /* Copy mod_key information to player input struct */
        player1_input->alt = alt_key_is_set();
        player1_input->ctrl = ctrl_key_is_set();
        player1_input->shift = shift_key_is_set();

        /* Logic */
        //test_cube_logic(player1_input, &test_pyramid);
        game_code.game_update(&game_state);

        /* Check for pointer grabbing changes */
        if (player1_input->pointer_state != player1_last_input->pointer_state)
        {
            set_pointer_state(player1_input, player1_input->pointer_state);
        }

        /* Rendering */

        // NOTE: This is just for testing that OpenGL actually works
        glClearColor(1.0f, 0.6f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        //draw_bouncing_cube_scene();
        //draw_static_cubes_scene(8);

        //draw_fps_counter();

        game_code.game_render(&game_state);


        glXSwapBuffers(x11_display, x11_window);

        ++game_state.framecount;
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

void build_programs(game_state_t *state)
{
    state->simple_color_program = make_gl_program(simple_color_vertex_shader_path, simple_color_fragment_shader_path);
}

/* Sets the VSync to either false or true.
   Returns 0 on error, 1 on success, and sets the "vsync" global accordingly.
   NOTE: It is possible for the system to still force VSync on our application
   outside our control. We might want to check for this later.
 */
int set_vsync(bool new_state)
{

    static PFNGLXSWAPINTERVALEXTPROC  glXSwapIntervalEXT  = 0;
    static PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = 0;
    static PFNGLXSWAPINTERVALSGIPROC  glXSwapIntervalSGI  = 0;

    static bool initialized = false;
    if (!initialized)
    {
        /* TODO: Check for ARB extensions before trying to get function pointers */
        glXSwapIntervalEXT  = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalEXT");
        glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalMESA");
        glXSwapIntervalSGI  = (PFNGLXSWAPINTERVALSGIPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalSGI");
    }

    if (vsync == new_state) /* Nothing to do */
    {
        return 1;
    }
    else if (new_state == false)
    {
        /* Disable Vsync */
        // TODO: Figure out why this does not work with different order
        // TODO: Error handling??

        if (glXSwapIntervalMESA)
        {
            glXSwapIntervalMESA(0);
            vsync = false;
        }
        if (glXSwapIntervalSGI)
        {
            glXSwapIntervalSGI(0);
            vsync = false;
        }
        if (glXSwapIntervalEXT)
        {
            GLXDrawable drawable = glXGetCurrentDrawable();
            glXSwapIntervalEXT(x11_display, drawable, 0);
            vsync = false;            return 1;
        }
        if (!glXSwapIntervalMESA && !glXSwapIntervalSGI && !glXSwapIntervalEXT)
        {
            return 0;
        }
        return 1;
    }
    else if (new_state == true)
    {
        if (glXSwapIntervalMESA)
        {
            glXSwapIntervalMESA(1);
            vsync = true;
        }
        if (glXSwapIntervalSGI)
        {
            glXSwapIntervalSGI(1);
            vsync = true;
        }
        if (glXSwapIntervalEXT)
        {
            GLXDrawable drawable = glXGetCurrentDrawable();
            glXSwapIntervalEXT(x11_display, drawable, 1);
            vsync = true;
        }
        if (!glXSwapIntervalMESA && !glXSwapIntervalSGI && !glXSwapIntervalEXT)
        {
            return 0;
        }
        return 1;
    }
    return 0; /* If we get here it's mostly an error */
}
