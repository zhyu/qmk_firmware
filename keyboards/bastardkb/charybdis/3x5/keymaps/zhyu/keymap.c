/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_SYMBOLS,
    LAYER_NAVNUM,
    LAYER_POINTER,
};

// Automatically enable sniping-mode on the pointer layer.
// #define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

#define _LSFT_HALF_ROW(K00, K01, K02, K03, K04) LSFT(K00), LSFT(K01), LSFT(K02), LSFT(K03), LSFT(K04)
#define _TRANSPARENT_HALF_ROW_ _______, _______, _______, _______, _______
#define _TRANSPARENT_LEFT_THUMB_CLUSTER_ _______, _______, _______
#define _TRANSPARENT_RIGHT_THUMB_CLUSTER_ _______, _______

// clang-format off
/** \brief QWERTY layout (3 rows, 10 columns). */
#define LAYOUT_LAYER_BASE                                                                                                        \
       KC_Q,            KC_W,    KC_E,    KC_R,    KC_T,               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,                 \
       LCTL_T(KC_A),    KC_S,    KC_D,    KC_F,    KC_G,               KC_H,    KC_J,    KC_K,    KC_L,    RCTL_T(KC_SEMICOLON), \
       LGUI_T(KC_Z),    KC_X,    KC_C,    KC_V,    KC_B,               KC_N,    KC_M, KC_COMM,  KC_DOT,    RGUI_T(KC_SLSH),      \
                      OSM(MOD_LSFT), KC_SPACE, OSL(LAYER_SYMBOLS),     OSL(LAYER_NAVNUM), KC_ENTER

/**
 * \brief Symbols layer.
 */
#define LAYOUT_LAYER_SYMBOLS                                                                                                                                      \
    _______, _______, LSFT(KC_GRAVE), LSFT(KC_QUOTE), _______,                               _______, KC_QUOTE, KC_GRAVE, _______, _______,                       \
    _LSFT_HALF_ROW(KC_1, KC_2, KC_3, KC_4, KC_5),                                            _LSFT_HALF_ROW(KC_6, KC_7, KC_8, KC_9, KC_0),                        \
    _LSFT_HALF_ROW(KC_LEFT_BRACKET, KC_RIGHT_BRACKET, KC_EQUAL, KC_MINUS, KC_BACKSLASH),     KC_BACKSLASH, KC_MINUS, KC_EQUAL, KC_LEFT_BRACKET, KC_RIGHT_BRACKET, \
                      _TRANSPARENT_LEFT_THUMB_CLUSTER_,                                      _______, KC_ESCAPE


/**
 * \brief Navigation and numeral layer.
 *
 * The left hand layer is the navigation layer.
 * The right hand layer is the numeral layer.
 */
#define LAYOUT_LAYER_NAVNUM                                                                              \
    KC_DELETE, KC_HOME, KC_UP, KC_END, C(S(A(KC_T))),          _______, KC_7, KC_8, KC_9, C(S(A(KC_P))), \
    KC_BACKSPACE, KC_LEFT, KC_DOWN, KC_RIGHT, _______,         _______, KC_4, KC_5, KC_6, KC_0,          \
    C(S(A(KC_SPACE))), _______, _______, _______, _______,     _______, KC_1, KC_2, KC_3, KC_DOT,        \
                      _______, KC_TAB, _______,                _TRANSPARENT_RIGHT_THUMB_CLUSTER_

/** \brief Mouse emulation and pointer functions. */
#define LAYOUT_LAYER_POINTER                                                                                                      \
    _TRANSPARENT_HALF_ROW_,                                 _TRANSPARENT_HALF_ROW_,                                               \
    _TRANSPARENT_HALF_ROW_,                                 KC_MS_BTN3, KC_MS_BTN1, KC_MS_BTN2, DRAGSCROLL_MODE_TOGGLE, _______,  \
    _TRANSPARENT_HALF_ROW_,                                 _TRANSPARENT_HALF_ROW_,                                               \
                      _TRANSPARENT_LEFT_THUMB_CLUSTER_,     _TRANSPARENT_RIGHT_THUMB_CLUSTER_

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_wrapper(LAYOUT_LAYER_BASE),
  [LAYER_SYMBOLS] = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
  [LAYER_NAVNUM] = LAYOUT_wrapper(LAYOUT_LAYER_NAVNUM),
  [LAYER_POINTER] = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in
// rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif

void shutdown_user(void) {
#ifdef RGBLIGHT_ENABLE
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_setrgb(RGB_RED);
#endif // RGBLIGHT_ENABLE
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_set_color_all(RGB_RED);
    rgb_matrix_update_pwm_buffers();
#endif // RGB_MATRIX_ENABLE
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(LAYER_POINTER);
    set_auto_mouse_enable(true); // always required before the auto mouse feature will work
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Disable the scroll mode when the highest layer is not the pointer layer
    if (get_highest_layer(state) != LAYER_POINTER) {
        charybdis_set_pointer_dragscroll_enabled(false);
    }
    return state;
}