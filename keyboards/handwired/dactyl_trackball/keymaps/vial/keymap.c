#include QMK_KEYBOARD_H
#include <stdio.h>
#include "pointing_device.h"

enum layers {
    _BASE,
    _LOWER
};

#define KEY_DRAG_SCROLL 0x7E00
#define KEY_DPI_UP      0x7E01
#define KEY_DPI_DOWN    0x7E02
#define KEY_SNIPING     0x7E03

#define SCROLL_DIVIDER 40

uint16_t current_cpi = PMW33XX_CPI;
static uint16_t saved_cpi = 0;

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

static bool is_drag_scroll = false;
static bool is_sniping = false;

void pointing_device_drag_scroll_event(bool scroll_active) {
    is_drag_scroll = scroll_active;
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
            oled_write_P(PSTR("LWR\n"), true);
            break;
        default:
            oled_write_P(PSTR("????\n"), false);
    }
	oled_write_P(PSTR("\n---\n"), false);
	oled_write_P(PSTR("MOUSE\n"), false); 	
	oled_write_P(PSTR("---\n"), false);
		
	oled_write_P(PSTR("DPI: "), false);
    uint16_t cpi = current_cpi;
    char cpi_str[6];
    sprintf(cpi_str, "%u", cpi);
    oled_write(cpi_str, false);

    oled_write_P(PSTR("\n\n"), false);
   

    if (is_drag_scroll) {
       oled_write_P(PSTR("SCRL"), true);
    } else {
       oled_write_P(PSTR("SCRL"), false);
    }
	oled_write_P(PSTR("\n"), false);
	if (is_sniping) {
       oled_write_P(PSTR("SNIP"), true);
    } else {
       oled_write_P(PSTR("SNIP"), false);
    }

    return false;
}

// 2. The Key Processing Logic
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KEY_DRAG_SCROLL:
            // When key is held, turn scroll flag ON. When released, OFF.
            if (record->event.pressed) {
                is_drag_scroll = true;
                // Optional: Update OLED listener if you use it
                pointing_device_drag_scroll_event(true); 
            } else {
                is_drag_scroll = false;
                pointing_device_drag_scroll_event(false);
            }
            return false;

        case KEY_DPI_UP:
            if (record->event.pressed) {
                // Increase by 400. Cap at 3200.
                if (current_cpi < 3200) {
                    current_cpi += 400;
                }
                pointing_device_set_cpi(current_cpi);
            }
            return false;

        // --- NEW DPI DOWN LOGIC ---
        case KEY_DPI_DOWN:
            if (record->event.pressed) {
                // Decrease by 400. Cap at 200.
                if (current_cpi > 400) {
                    current_cpi -= 400;
                }
                pointing_device_set_cpi(current_cpi);
            }
            return false;
			
		case KEY_SNIPING:
            if (record->event.pressed) {
                // 1. Save the current normal speed
                saved_cpi = current_cpi;
                // 2. Set sensor to Low Speed (e.g. 200)
                pointing_device_set_cpi(200);
				
				is_sniping = true;
            } else {
                // 1. Restore the normal speed
                if (saved_cpi > 0) { // Safety check
                    pointing_device_set_cpi(saved_cpi);
                    saved_cpi = 0; // Reset flag
                }
				else {
					pointing_device_set_cpi(800);
				}
				
				is_sniping = false;
            }
            return false;
    }
    return true;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (is_drag_scroll) {
        static int16_t scroll_h_accum = 0;
        static int16_t scroll_v_accum = 0;
        
        scroll_h_accum += mouse_report.x;
        scroll_v_accum += mouse_report.y;

        // INVERT HORIZONTAL (Optional - remove the minus if you like it normal)
        mouse_report.h = -(scroll_h_accum / SCROLL_DIVIDER);

        // INVERT VERTICAL (This creates "Natural Scrolling")
        mouse_report.v = -(scroll_v_accum / SCROLL_DIVIDER);
        
        // Keep the remainder for smoothness
        scroll_h_accum %= SCROLL_DIVIDER;
        scroll_v_accum %= SCROLL_DIVIDER;
        
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}