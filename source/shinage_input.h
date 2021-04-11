#ifndef SHINAGE_INPUT_H
#define SHINAGE_INPUT_H

/* Input handling macros */
#define CTRL_MOD_KEY (1 << 0)
#define SHIFT_MOD_KEY (1 << 1)
#define ALT_MOD_KEY (1 << 2)

/* NOTE: Adding more descriptive aliases for X11's button macros.
   There are some more buttons not described by the X11 header that
   we might want to define here, too.
 */

// These are just unsigned ints
#define LEFT_MOUSE_BUTTON        Button1 // 1
#define RIGHT_MOUSE_BUTTON       Button3 // 3
#define WHEEL_PRESS_MOUSE_BUTTON Button2 // 2
#define WHEEL_UP_MOUSE_BUTTON    Button4 // 4
#define WHEEL_DOWN_MOUSE_BUTTON  Button5 // 5

typedef enum {
    PLAYER_1 = 0, PLAYER_2 = 1, PLAYER_3 = 2, PLAYER_4 = 3, MAX_PLAYERS = 4
} player_idx_t;


/* Input handling types */
// TODO: complete input
// TODO: extra data structure for input remapping info

/* TODO: Right now we only save a frame of history input. This is *not* enough for
   complex motions or for distinguishing clicks from holds.
   We may want to extend the number of frames we take in the history.
*/

typedef enum {
  // Unactive
  UNPRESSED     = 0,
  /* Active, used when we don't need additional logic for distinguishing
     rapid presses or holding */
  PRESSED       = 1,
  /* Active and being held for at least a frame. Binary representation
   is a bitmask so HOLDING always implies PRESSED */
  HOLDING       = 0b11,
  /* Active, but the key is only meant to invoke a single action the first frame
     that it is pressed, then ignored. Binary representation is a bitmask
     so JUST_PRESSED always implies PRESSED */
  JUST_PRESSED  = 0b101,
  /* Unactive, but was being pressed until this frame. Binary representation is a
   bitmask so JUST_RELEASED implies UNPRESSED*/
  JUST_RELEASED = 0b1000
} key_state_t;

typedef struct {
  // The state of the key, see the enum above
  int    state;
  /* This is a clock value in seconds indicating the last time that the key
     _started_ being pressed. Used for complex inputs */
  double stamp; 
} key_input_t;

typedef struct {
  union {
    struct {
      key_input_t                up; // 1
      key_input_t              down; // 2
      key_input_t              left; // 3
      key_input_t             right; // 4
      key_input_t           forward; // 5
      key_input_t              back; // 6
      key_input_t  mouse_left_click; // 7
      key_input_t mouse_right_click; // 8
    };
    /* HACK: This has to be the same size as the struct above. See
     consume_first_presses below. */
    key_input_t key_records[8];
  };
  /* NOTE: What are these coords relative to? Whatever it is, it should be
     consistent between platforms */
  unsigned int         cursor_x;
  unsigned int         cursor_y;
  /* These are redundant but make input passing easier so we'll keeping them as a test */
  int cursor_x_delta;
  int cursor_y_delta;
  /* NOTE: The modifier keys do not seem to need the complicated handling done to others.
     Their state will always be correct as soon as we get one event in a particular frame. */
  bool                      alt;
  bool                     ctrl;
  bool                    shift;
} player_input_t;

/* Input handling globals */
uint8_t modifier_keys = 0;
char mod_key_str[64];

player_input_t _curr_frame_input[MAX_PLAYERS]; 
player_input_t _last_frame_input[MAX_PLAYERS];

player_input_t *curr_frame_input = &_curr_frame_input[0];
player_input_t *last_frame_input = &_last_frame_input[0];


const player_input_t empty_player_input = {};

/* Input handling inlines */
static inline bool  ctrl_key_is_set() { return modifier_keys &  CTRL_MOD_KEY; };
static inline bool shift_key_is_set() { return modifier_keys & SHIFT_MOD_KEY; };
static inline bool   alt_key_is_set() { return modifier_keys &   ALT_MOD_KEY; };

static inline void  set_ctrl_key() { modifier_keys |=  CTRL_MOD_KEY; };
static inline void set_shift_key() { modifier_keys |= SHIFT_MOD_KEY; };
static inline void   set_alt_key() { modifier_keys |=   ALT_MOD_KEY; };

static inline void  unset_ctrl_key() { modifier_keys &=  !CTRL_MOD_KEY; };
static inline void unset_shift_key() { modifier_keys &= !SHIFT_MOD_KEY; };
static inline void   unset_alt_key() { modifier_keys &=   !ALT_MOD_KEY; };

static inline char *mod_key_str_prefix()
{
    char *s = mod_key_str;
    *s = 0;
    if (ctrl_key_is_set())
        s += sprintf(s, "Ctrl + ");

    if (shift_key_is_set())
        s += sprintf(s, "Shift + ");

    if (alt_key_is_set())
        s += sprintf(s, "Alt + ");

    return mod_key_str;
}

static inline void dispatch_mod_keys(unsigned int modifier_keys_bitmask)
{
    if (modifier_keys_bitmask & ShiftMask)
        set_shift_key();
    if (modifier_keys_bitmask & ControlMask)
        set_ctrl_key();
    if (modifier_keys_bitmask & Mod1Mask) // Mod1 is, usually, the Alt key
        set_alt_key();
}


/* Sets the current state of input struct in the present frame based on its current state
   and its history. */
void set_input_state(key_input_t *curr_input, key_input_t *last_input, int curr_state, double curr_timestamp)
{
    if (curr_state == PRESSED)
    {
        /* Case 1: The key is pressed now, but it was not being pressed before */
        if (!(last_input->state & PRESSED))
        {
            /* NOTE: We only change timestamp here. We're only interested in the moment of the initial activation */
            curr_input->state = JUST_PRESSED;
            curr_input->stamp = curr_timestamp;
        }
        /* Case 2: The key is pressed now, and it was already being pressed */
        else
        {
            curr_input->state = HOLDING;
        }
    }

    
    if (curr_state == UNPRESSED)
    {
        /* Case 3: the key is not being pressed, but it was being pressed before */    
        if (last_input->state & PRESSED)
        {
            curr_input->state = JUST_RELEASED;
        }
        /* Case 4: the key is not being pressed, and it stopped being pressed
           just the frame before. */
        
        else if (last_input->state & JUST_RELEASED)
        {
            curr_input->state = UNPRESSED;
        }
    }
}

/* HACK: See player_input_t above. At the start of each frame
   and before the X11 event processing we have to visit all our
   key records and delete any JUST_X entries to make sure we
   don't loop between states in the absence of a X11 input event. */

static inline void consume_first_presses(player_input_t *p)
{
    int nrecords = sizeof(p->key_records)/sizeof(p->key_records[0]);
    for (int i = 0; i < nrecords; ++i)
    {
        if (p->key_records[i].state == JUST_PRESSED)
            p->key_records[i].state = HOLDING;
        
        if (p->key_records[i].state == JUST_RELEASED)
            p->key_records[i].state = UNPRESSED;
    }
}

/* Inline functions to query the current state of a key. 
   NOTE: It is *highly* advisable to only use these instead of querying the 
   .state member directly. Its internal representation can lead to confusing results. */

static inline bool is_pressed(key_input_t key)
{
    return (key.state & PRESSED ? true : false);
}

static inline bool is_held(key_input_t key)
{
    return (key.state & HOLDING ? true : false);
}

static inline bool is_just_released(key_input_t key)
{
    return (key.state & JUST_RELEASED ? true : false);
}

static inline bool is_just_pressed(key_input_t key)
{
    return (key.state == JUST_PRESSED ? true : false);
}

#endif
