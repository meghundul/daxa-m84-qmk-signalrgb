#include QMK_KEYBOARD_H
 
enum layers {
    _BASE,
    _FN,
};
 
#ifdef RGB_MATRIX_ENABLE
uint8_t signalrgb_colors[84][3];
bool signalrgb_active = false;
 
// Hardware-level color calibration (replaces SignalRGB Pro)
// Values derived from the #7dffc5 pure white test
#define RED_BALANCE  125
#define BLUE_BALANCE 197
#endif
 
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_84_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR, KC_PAUS, KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,            KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(_FN), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FN] = LAYOUT_84_ansi(
        QK_BOOT, LGUI(KC_D), KC_MYCM, KC_MAIL, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_VOLU, KC_VOLD, KC_MUTE, KC_CALC, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          RM_NEXT,
        GU_TOGG, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          RM_TOGG,
        KC_TRNS, AG_NORM,    AG_SWAP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS,
        KC_TRNS,             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RM_VALU,          KC_TRNS,
        KC_TRNS, LGUI(KC_L), KC_TRNS,                            KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, RM_SPDD, RM_VALD, RM_SPDU
    ),
};

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (signalrgb_active) {
        for (uint8_t i = led_min; i < led_max; i++) {
            if (i < 84) {
                rgb_matrix_set_color(i, signalrgb_colors[i][0], signalrgb_colors[i][1], signalrgb_colors[i][2]);
            }
        }
    }
 
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_LOCK_LED_INDEX, 255, 255, 255);
    }
 
    return false;
}
#endif
 
#ifdef RAW_ENABLE
#include "raw_hid.h"
 
void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (data[0] == 0x01) {
        signalrgb_active = true;
        rgb_matrix_enable_noeeprom();
        rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv_noeeprom(0, 0, 255);
    }
    else if (data[0] == 0x02) {
        uint8_t start_idx = data[1];
        uint8_t count = data[2];
        for (uint8_t i = 0; i < count; i++) {
            if (start_idx + i < 84) {
                // THE MYSTERY IS SOLVED: The USB data is arriving in GRB order!
                // data[3] is Green, data[4] is Red, data[5] is Blue.
                
                uint8_t raw_g = data[3 + (i * 3)]; // Green
                uint8_t raw_r = data[4 + (i * 3)]; // Red
                uint8_t raw_b = data[5 + (i * 3)]; // Blue
                
                // Apply the #7dffc5 balance (Red=125, Green=255, Blue=197) to the CORRECT channels
                signalrgb_colors[start_idx + i][0] = raw_g; // Green stays 100%
                signalrgb_colors[start_idx + i][1] = (uint8_t)(((uint16_t)raw_r * RED_BALANCE) / 255); // Scale Red down
                signalrgb_colors[start_idx + i][2] = (uint8_t)(((uint16_t)raw_b * BLUE_BALANCE) / 255); // Scale Blue down
            }
        }
    }
    else if (data[0] == 0x04) {
        signalrgb_active = false;
        rgb_matrix_reload_from_eeprom();
    }
}
#endif