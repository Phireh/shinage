## Compilation
    - `make [shinage]` for game
    - `make tests` for unit tests
    
## Dependencies (so far)
    - `gcc` (for compiling)
    - `ld` (for linking)
    - `glibc` (for math headers, timing, strings, etc)
    - `X11 and headers` (for window/input)
    - `OpenGL >=3.2` (for GPU rendering. We're planning on bumping it to 4.0 for compute shaders)
    - `GLX` (for OpenGL extensions, context creation, etc)
    - `freetype2` (for font rendering)
    - `GNU Make` (for build system on Linux)
    - `pkg-config` (for retrieving dependency info)
    - `bash` (if using GNU global)
    - `awk` (for Makefile rules, if using tags)
    
## Tagging
    - `TAGS_FLAVOR=<ctags/etags> make tags` for ctags/etags
    - `make gtags` for GNU Global
    
## Controls (for cube test)
    - Mouse: move the camera
    - Left click: reset camera to center
    - Right click: print camera matrix info
    - Q,E: roll left, right
    - A,D: move left, right
    - W,S: move forward, back
    - R,F: move up, down
    - Space: print camera position
    - F1: grab mouse pointer
    - F2: lock camera roll
    - ESC: close game
    
## TODOs
    - Basic 3D texels
    - Lights
    - Physics
    - Texture support
    - GUI
    - Dynamic code reloading
    - Sound
    - Windows platform layer
    - Input remapping
    - Input recording
    - Debug menu
    - Memory arenas/manual memory allocation
    - Remove / parametrize random debug code
    - Further testing
    - More thorough error handling
    - Ugly code cleanup
