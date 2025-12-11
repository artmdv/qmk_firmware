#include QMK_KEYBOARD_H
#include <stdio.h>

enum layers {
    _BASE,
    _LOWER
};

// --------------------------------------------------------------------------
// KEYMAP
// --------------------------------------------------------------------------
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        // LEFT SIDE (Your previous layout)
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,     
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     
        KC_LCTL, MT(MOD_LSFT, KC_Z), MT(MOD_LALT, KC_X), MT(MOD_LGUI, KC_C), KC_V, KC_B, 
        KC_INSERT,   KC_HOME,   KC_PGUP,   KC_ENTER,   KC_END,   KC_PGDN,   
                 KC_BSPC, KC_DEL,  MO(_LOWER), KC_SPC,

        // RIGHT SIDE (New!)
        // I mirrored the layout. Row 5 contains the special keys you mentioned.
        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        
        // Right Bottom Row (Row 5)
        // You said "Key 6 at bottom of C and D" and "4 keys in thumbcluster"
        // I put Mouse Buttons here for the Trackball!
        KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_BTN1, KC_BTN2 
    ),

    [_LOWER] = LAYOUT(
        // LEFT SIDE
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
        KC_TRNS, KC_TRNS, KC_UP,   KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_HOME, KC_END,  KC_TRNS, KC_ENT,

        // RIGHT SIDE (Function Keys & Navigation)
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        
        // Right Bottom Row
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN3, KC_BTN4
    )
};

// --------------------------------------------------------------------------
// OLED CONFIGURATION
// --------------------------------------------------------------------------

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

bool oled_task_user(void) {
    // -------------------------------------------------------
    // Vertical Layout (32 pixels wide x 128 pixels tall)
    // -------------------------------------------------------
    
    // Header
    oled_write_P(PSTR("LAYER\n"), false); 
    oled_write_P(PSTR("---\n"), false);

    // Layer Name
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("BASE\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("LOWER\n"), true);
            break;
        default:
            oled_write_P(PSTR("????\n"), false);
    }
	
	
    
    oled_write_P(PSTR("\n"), false);

    // Modifiers (Compact Vertical Stack)
    uint8_t mods = get_mods();

    // C = Ctrl, S = Shift, A = Alt, G = GUI
    if (mods & MOD_MASK_CTRL)  oled_write_P(PSTR("CTRL\n"), true);
    else                       oled_write_P(PSTR("CTRL\n"), false);

    if (mods & MOD_MASK_SHIFT) oled_write_P(PSTR("SHIFT\n"), true);
    else                       oled_write_P(PSTR("SHIFT\n"), false);

    if (mods & MOD_MASK_ALT)   oled_write_P(PSTR("ALT\n"), true);
    else                       oled_write_P(PSTR("ALT\n"), false);
    
    if (mods & MOD_MASK_GUI)   oled_write_P(PSTR("WIN\n"), true);
    else                       oled_write_P(PSTR("WIN\n"), false);

    return false;
}