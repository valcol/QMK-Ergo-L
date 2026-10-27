// Ergo-L (https://ergol.org) emulé dans le firmware pour un hôte AZERTY.
// SPDX-License-Identifier: GPL-2.0-or-later
//
// L'ordinateur reste configuré en AZERTY français : le clavier envoie les
// touches AZERTY qui produisent les caractères Ergo-L. Trois hôtes sont gérés,
// au choix avec Nav+Sym puis W (Windows), L (Linux) ou M (macOS) :
//   - Windows : AZERTY « Français (France) », codes Alt pour œ « » É …
//   - Linux   : xkb « fr » (basic), AltGr + Ctrl+Maj+U pour l'Unicode restant
//   - macOS   : « Français » (pas « Français – PC »), touches Option

#include QMK_KEYBOARD_H
#include "keymap_french.h"

enum layers { _BASE, _ODK, _SYM, _NAV, _ADJ };

enum hosts { HOST_WIN, HOST_LNX, HOST_MAC, HOST_COUNT };

/******************************************************************************
 * Caractères : une sortie par hôte
 ******************************************************************************/

enum out_types {
    O_KEY,  // touche (avec modificateurs), répétée par l'OS
    O_SEQ,  // deux frappes : touche morte + touche
    O_CAPS, // Verr Maj + touche (É È À Ù Ç sous Linux/macOS)
    O_ALT,  // code Alt Windows 0nnn (CP1252)
    O_UNI,  // Unicode Linux : Ctrl+Maj+U, hexa, espace
};

typedef struct {
    uint8_t  type;
    uint16_t a;
    uint16_t b;
} out_t;

#define K(kc) {O_KEY, (kc), 0}
#define SEQ(a, b) {O_SEQ, (a), (b)}
#define CAPS(kc) {O_CAPS, (kc), 0}
#define ALTC(n) {O_ALT, (n), 0}
#define UNI(cp) {O_UNI, (cp), 0}
#define ALL(o) o, o, o

#define FR_CARET ALGR(FR_CCED) // ^ non mort (AltGr+9)

enum chars {
    CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_0,
    CH_EURO, CH_LAQ, CH_RAQ, CH_DLR, CH_PERC, CH_CIRC, CH_AMPR, CH_ASTR, CH_HASH, CH_AT,
    CH_MINS, CH_QUES, CH_DOT, CH_COLN, CH_COMM, CH_SCLN, CH_EXLM,
    CH_LT, CH_GT, CH_QUOT, CH_GRV, CH_LCBR, CH_LPRN, CH_RPRN, CH_RCBR, CH_EQL,
    CH_BSLS, CH_PLUS, CH_SLSH, CH_DQUO, CH_TILD, CH_LBRC, CH_RBRC, CH_UNDS, CH_PIPE,
    // minuscule / majuscule
    CH_ACIR, CH_ACIR_U, CH_ECIR, CH_ECIR_U, CH_ICIR, CH_ICIR_U, CH_OCIR, CH_OCIR_U,
    CH_UCIR, CH_UCIR_U, CH_IDIA, CH_IDIA_U, CH_AGRV, CH_AGRV_U, CH_EACU, CH_EACU_U,
    CH_EGRV, CH_EGRV_U, CH_UGRV, CH_UGRV_U, CH_CCED, CH_CCED_U, CH_OE, CH_OE_U,
    CH_AE, CH_AE_U, CH_NTIL, CH_NTIL_U,
    CH_SZ, CH_MICR, CH_DIAE, CH_IEXL, CH_IQUE, CH_NDASH, CH_MDASH, CH_ELLIP,
    CH_MDOT, CH_BULL, CH_RSQUO,
    // raccourcis (Ctrl sous Windows/Linux, Cmd sous macOS)
    CH_UNDO, CH_CUT, CH_COPY, CH_PASTE, CH_REDO, CH_SELALL,
    CH_COUNT
};

// clang-format off
static const out_t chars[CH_COUNT][HOST_COUNT] = {
    //                Windows                   Linux                      macOS
    [CH_1]      = { ALL(K(FR_1)) },
    [CH_2]      = { ALL(K(FR_2)) },
    [CH_3]      = { ALL(K(FR_3)) },
    [CH_4]      = { ALL(K(FR_4)) },
    [CH_5]      = { ALL(K(FR_5)) },
    [CH_6]      = { ALL(K(FR_6)) },
    [CH_7]      = { ALL(K(FR_7)) },
    [CH_8]      = { ALL(K(FR_8)) },
    [CH_9]      = { ALL(K(FR_9)) },
    [CH_0]      = { ALL(K(FR_0)) },

    [CH_EURO]   = { K(FR_EURO),                K(FR_EURO),                K(RALT(KC_RBRC))           },
    [CH_LAQ]    = { ALTC(171),                 K(RALT(KC_W)),             K(RALT(KC_7))              },
    [CH_RAQ]    = { ALTC(187),                 K(RALT(KC_X)),             K(RALT(S(KC_7)))           },
    [CH_DLR]    = { ALL(K(FR_DLR)) },
    [CH_PERC]   = { ALL(K(FR_PERC)) },
    [CH_CIRC]   = { K(FR_CARET),               K(FR_CARET),               SEQ(FR_CIRC, KC_SPC)       },
    [CH_AMPR]   = { ALL(K(FR_AMPR)) },
    [CH_ASTR]   = { K(FR_ASTR),                K(FR_ASTR),                K(S(KC_RBRC))              },
    [CH_HASH]   = { K(FR_HASH),                K(FR_HASH),                K(S(KC_NUBS))              },
    [CH_AT]     = { K(FR_AT),                  K(FR_AT),                  K(KC_NUBS)                 },

    [CH_MINS]   = { K(FR_MINS),                K(FR_MINS),                K(KC_EQL)                  },
    [CH_QUES]   = { ALL(K(FR_QUES)) },
    [CH_DOT]    = { ALL(K(FR_DOT)) },
    [CH_COLN]   = { ALL(K(FR_COLN)) },
    [CH_COMM]   = { ALL(K(FR_COMM)) },
    [CH_SCLN]   = { ALL(K(FR_SCLN)) },
    [CH_EXLM]   = { K(FR_EXLM),                K(FR_EXLM),                K(KC_8)                    },

    [CH_LT]     = { K(FR_LABK),                K(FR_LABK),                K(KC_GRV)                  },
    [CH_GT]     = { K(FR_RABK),                K(FR_RABK),                K(S(KC_GRV))               },
    [CH_QUOT]   = { ALL(K(FR_QUOT)) },
    [CH_GRV]    = { SEQ(FR_GRV, KC_SPC),       K(FR_GRV),                 SEQ(KC_NUHS, KC_SPC)       },
    [CH_LCBR]   = { K(FR_LCBR),                K(FR_LCBR),                K(RALT(KC_5))              },
    [CH_LPRN]   = { ALL(K(FR_LPRN)) },
    [CH_RPRN]   = { ALL(K(FR_RPRN)) },
    [CH_RCBR]   = { K(FR_RCBR),                K(FR_RCBR),                K(RALT(KC_MINS))           },
    [CH_EQL]    = { K(FR_EQL),                 K(FR_EQL),                 K(KC_SLSH)                 },
    [CH_BSLS]   = { K(FR_BSLS),                K(FR_BSLS),                K(RALT(S(KC_DOT)))         },
    [CH_PLUS]   = { K(FR_PLUS),                K(FR_PLUS),                K(S(KC_SLSH))              },
    [CH_SLSH]   = { ALL(K(FR_SLSH)) },
    [CH_DQUO]   = { ALL(K(FR_DQUO)) },
    [CH_TILD]   = { SEQ(FR_TILD, KC_SPC),      K(FR_TILD),                SEQ(RALT(KC_N), KC_SPC)    },
    [CH_LBRC]   = { K(FR_LBRC),                K(FR_LBRC),                K(RALT(S(KC_5)))           },
    [CH_RBRC]   = { K(FR_RBRC),                K(FR_RBRC),                K(RALT(S(KC_MINS)))        },
    [CH_UNDS]   = { K(FR_UNDS),                K(FR_UNDS),                K(S(KC_EQL))               },
    [CH_PIPE]   = { K(FR_PIPE),                K(FR_PIPE),                K(RALT(S(KC_L)))           },

    [CH_ACIR]   = { ALL(SEQ(FR_CIRC, FR_A)) },
    [CH_ACIR_U] = { ALL(SEQ(FR_CIRC, S(FR_A))) },
    [CH_ECIR]   = { ALL(SEQ(FR_CIRC, FR_E)) },
    [CH_ECIR_U] = { ALL(SEQ(FR_CIRC, S(FR_E))) },
    [CH_ICIR]   = { ALL(SEQ(FR_CIRC, FR_I)) },
    [CH_ICIR_U] = { ALL(SEQ(FR_CIRC, S(FR_I))) },
    [CH_OCIR]   = { ALL(SEQ(FR_CIRC, FR_O)) },
    [CH_OCIR_U] = { ALL(SEQ(FR_CIRC, S(FR_O))) },
    [CH_UCIR]   = { ALL(SEQ(FR_CIRC, FR_U)) },
    [CH_UCIR_U] = { ALL(SEQ(FR_CIRC, S(FR_U))) },
    [CH_IDIA]   = { ALL(SEQ(FR_DIAE, FR_I)) },
    [CH_IDIA_U] = { ALL(SEQ(FR_DIAE, S(FR_I))) },
    [CH_AGRV]   = { ALL(K(FR_AGRV)) },
    [CH_AGRV_U] = { ALTC(192),                 CAPS(FR_AGRV),             CAPS(FR_AGRV)              },
    [CH_EACU]   = { ALL(K(FR_EACU)) },
    [CH_EACU_U] = { ALTC(201),                 CAPS(FR_EACU),             CAPS(FR_EACU)              },
    [CH_EGRV]   = { ALL(K(FR_EGRV)) },
    [CH_EGRV_U] = { ALTC(200),                 CAPS(FR_EGRV),             CAPS(FR_EGRV)              },
    [CH_UGRV]   = { ALL(K(FR_UGRV)) },
    [CH_UGRV_U] = { ALTC(217),                 CAPS(FR_UGRV),             CAPS(FR_UGRV)              },
    [CH_CCED]   = { ALL(K(FR_CCED)) },
    [CH_CCED_U] = { ALTC(199),                 CAPS(FR_CCED),             CAPS(FR_CCED)              },
    [CH_OE]     = { ALTC(156),                 UNI(0x0153),               K(RALT(KC_O))              },
    [CH_OE_U]   = { ALTC(140),                 UNI(0x0152),               K(RALT(S(KC_O)))           },
    [CH_AE]     = { ALTC(230),                 K(RALT(KC_Q)),             K(RALT(KC_Q))              },
    [CH_AE_U]   = { ALTC(198),                 K(RALT(S(KC_Q))),          K(RALT(S(KC_Q)))           },
    [CH_NTIL]   = { ALTC(241),                 UNI(0x00F1),               SEQ(RALT(KC_N), KC_N)      },
    [CH_NTIL_U] = { ALTC(209),                 UNI(0x00D1),               SEQ(RALT(KC_N), S(KC_N))   },

    [CH_SZ]     = { ALTC(223),                 K(RALT(KC_S)),             K(RALT(KC_B))              },
    [CH_MICR]   = { K(FR_MICR),                K(FR_MICR),                K(RALT(KC_SCLN))           },
    [CH_DIAE]   = { ALL(K(FR_DIAE)) },
    [CH_IEXL]   = { ALTC(161),                 K(RALT(S(KC_1))),          K(RALT(KC_8))              },
    [CH_IQUE]   = { ALTC(191),                 K(RALT(S(KC_MINS))),       K(RALT(S(KC_M)))           },
    [CH_NDASH]  = { ALTC(150),                 UNI(0x2013),               K(RALT(S(KC_EQL)))         },
    [CH_MDASH]  = { ALTC(151),                 UNI(0x2014),               K(RALT(KC_EQL))            },
    [CH_ELLIP]  = { ALTC(133),                 UNI(0x2026),               K(RALT(KC_COMM))           },
    [CH_MDOT]   = { ALTC(183),                 K(RALT(KC_DOT)),           K(RALT(S(KC_F)))           },
    [CH_BULL]   = { ALTC(149),                 K(RALT(KC_COMM)),          K(RALT(S(KC_COMM)))        },
    [CH_RSQUO]  = { ALTC(146),                 K(RALT(S(KC_N))),          K(RALT(S(KC_4)))           },

    [CH_UNDO]   = { K(C(FR_Z)),                K(C(FR_Z)),                K(G(FR_Z))                 },
    [CH_CUT]    = { K(C(FR_X)),                K(C(FR_X)),                K(G(FR_X))                 },
    [CH_COPY]   = { K(C(FR_C)),                K(C(FR_C)),                K(G(FR_C))                 },
    [CH_PASTE]  = { K(C(FR_V)),                K(C(FR_V)),                K(G(FR_V))                 },
    [CH_REDO]   = { K(C(FR_Y)),                K(C(S(FR_Z))),             K(G(S(FR_Z)))              },
    [CH_SELALL] = { K(C(FR_A)),                K(C(FR_A)),                K(G(FR_A))                 },
};
// clang-format on

/******************************************************************************
 * Keycodes
 ******************************************************************************/

enum custom_keycodes {
    ERG_ODK = SAFE_RANGE, // touche morte Ergo-L ★ (Maj : !)
    HST_WIN,
    HST_LNX,
    HST_MAC,

    // Touches à deux niveaux : une entrée par ligne de erg_keys[]
    ERG_FIRST,
    ERG_1 = ERG_FIRST,
    ERG_2,
    ERG_3,
    ERG_4,
    ERG_5,
    ERG_6,
    ERG_7,
    ERG_8,
    ERG_9,
    ERG_0,
    ERG_MINS,
    ERG_DOT,
    ERG_COMM,
    ODK_A,
    ODK_C,
    ODK_O,
    ODK_P,
    ODK_ODK,
    ODK_Y,
    ODK_Q,
    ODK_S,
    ODK_E,
    ODK_N,
    ODK_F,
    ODK_T,
    ODK_I,
    ODK_U,
    ODK_Z,
    ODK_MINS,
    ODK_COMM,
    ERG_LAST,

    // Touches à un seul caractère : CK(CH_xxx)
    CHAR_FIRST = ERG_LAST,
};

#define CK(ch) (CHAR_FIRST + (ch))
#define CHAR_LAST CK(CH_COUNT)

_Static_assert(CHAR_LAST <= QK_USER_MAX, "trop de keycodes");

// {sans Maj, avec Maj}
static const uint8_t erg_keys[][2] = {
    [ERG_1 - ERG_FIRST]    = {CH_1, CH_EURO},
    [ERG_2 - ERG_FIRST]    = {CH_2, CH_LAQ},
    [ERG_3 - ERG_FIRST]    = {CH_3, CH_RAQ},
    [ERG_4 - ERG_FIRST]    = {CH_4, CH_DLR},
    [ERG_5 - ERG_FIRST]    = {CH_5, CH_PERC},
    [ERG_6 - ERG_FIRST]    = {CH_6, CH_CIRC},
    [ERG_7 - ERG_FIRST]    = {CH_7, CH_AMPR},
    [ERG_8 - ERG_FIRST]    = {CH_8, CH_ASTR},
    [ERG_9 - ERG_FIRST]    = {CH_9, CH_HASH},
    [ERG_0 - ERG_FIRST]    = {CH_0, CH_AT},
    [ERG_MINS - ERG_FIRST] = {CH_MINS, CH_QUES},
    [ERG_DOT - ERG_FIRST]  = {CH_DOT, CH_COLN},
    [ERG_COMM - ERG_FIRST] = {CH_COMM, CH_SCLN},
    [ODK_A - ERG_FIRST]    = {CH_ACIR, CH_ACIR_U},
    [ODK_C - ERG_FIRST]    = {CH_CCED, CH_CCED_U},
    [ODK_O - ERG_FIRST]    = {CH_OE, CH_OE_U},
    [ODK_P - ERG_FIRST]    = {CH_OCIR, CH_OCIR_U},
    [ODK_ODK - ERG_FIRST]  = {CH_DIAE, CH_IEXL},
    [ODK_Y - ERG_FIRST]    = {CH_UCIR, CH_UCIR_U},
    [ODK_Q - ERG_FIRST]    = {CH_AGRV, CH_AGRV_U},
    [ODK_S - ERG_FIRST]    = {CH_EACU, CH_EACU_U},
    [ODK_E - ERG_FIRST]    = {CH_EGRV, CH_EGRV_U},
    [ODK_N - ERG_FIRST]    = {CH_ECIR, CH_ECIR_U},
    [ODK_F - ERG_FIRST]    = {CH_NTIL, CH_NTIL_U},
    [ODK_T - ERG_FIRST]    = {CH_ICIR, CH_ICIR_U},
    [ODK_I - ERG_FIRST]    = {CH_IDIA, CH_IDIA_U},
    [ODK_U - ERG_FIRST]    = {CH_UGRV, CH_UGRV_U},
    [ODK_Z - ERG_FIRST]    = {CH_AE, CH_AE_U},
    [ODK_MINS - ERG_FIRST] = {CH_MINS, CH_IQUE},
    [ODK_COMM - ERG_FIRST] = {CH_MDOT, CH_BULL},
};

_Static_assert(sizeof(erg_keys) / sizeof(erg_keys[0]) == ERG_LAST - ERG_FIRST, "erg_keys[] incomplet");

#define NAV_BSPC LT(_NAV, KC_BSPC)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Ergo-L
    //  Esc  1  2  3  4  5      6  7  8  9  0  Suppr
    //  Tab  q  c  o  p  w      j  m  d  ★  y  RetArr
    //  Ctrl a  s  e  n  f      l  r  t  i  u  Entrée
    //  Maj  z  x  -  v  b      .  h  g  ,  k  Maj
    //          Win Maj Nav/⌫   Espace Sym Alt
    [_BASE] = LAYOUT(
        KC_ESC,  ERG_1,   ERG_2,   ERG_3,    ERG_4,   ERG_5,                     ERG_6,   ERG_7,   ERG_8,   ERG_9,    ERG_0,   KC_DEL,
        KC_TAB,  FR_Q,    FR_C,    FR_O,     FR_P,    FR_W,                      FR_J,    FR_M,    FR_D,    ERG_ODK,  FR_Y,    KC_BSPC,
        KC_LCTL, FR_A,    FR_S,    FR_E,     FR_N,    FR_F,                      FR_L,    FR_R,    FR_T,    FR_I,     FR_U,    KC_ENT,
        KC_LSFT, FR_Z,    FR_X,    ERG_MINS, FR_V,    FR_B,                      ERG_DOT, FR_H,    FR_G,    ERG_COMM, FR_K,    KC_RSFT,
                                             KC_LGUI, KC_LSFT, NAV_BSPC,  KC_SPC, MO(_SYM), KC_LALT
    ),
    // Touche morte ★ (one-shot), □ = touche de base
    //  â ç œ ô □      □ µ _ ¨ û
    //  à é è ê ñ      ( ) î ï ù
    //  æ ß - – —      … □ □ · □      (Maj+, : •)
    //  Espace : ’
    [_ODK] = LAYOUT(
        _______, _______, _______, _______,  _______,      _______,                  _______,      _______,      _______,     _______,      _______, _______,
        _______, ODK_A,   ODK_C,   ODK_O,    ODK_P,        _______,                  _______,      CK(CH_MICR),  CK(CH_UNDS), ODK_ODK,      ODK_Y,   _______,
        _______, ODK_Q,   ODK_S,   ODK_E,    ODK_N,        ODK_F,                    CK(CH_LPRN),  CK(CH_RPRN),  ODK_T,       ODK_I,        ODK_U,   _______,
        _______, ODK_Z,   CK(CH_SZ), ODK_MINS, CK(CH_NDASH), CK(CH_MDASH),           CK(CH_ELLIP), _______,      _______,     ODK_COMM,     _______, _______,
                                             _______,      _______, _______,  CK(CH_RSQUO), _______, _______
    ),
    // Symboles (équivalent de la couche AltGr d'Ergo-L)
    //  ^ < > $ %      @ & * ' `
    //  { ( ) } =      \ + - / "
    //  ~ [ ] _ #      | ! ; : ?
    [_SYM] = LAYOUT(
        _______, _______,     _______,     _______,     _______,     _______,          _______,     _______,     _______,     _______,     _______,     _______,
        _______, CK(CH_CIRC), CK(CH_LT),   CK(CH_GT),   CK(CH_DLR),  CK(CH_PERC),      CK(CH_AT),   CK(CH_AMPR), CK(CH_ASTR), CK(CH_QUOT), CK(CH_GRV),  _______,
        _______, CK(CH_LCBR), CK(CH_LPRN), CK(CH_RPRN), CK(CH_RCBR), CK(CH_EQL),       CK(CH_BSLS), CK(CH_PLUS), CK(CH_MINS), CK(CH_SLSH), CK(CH_DQUO), _______,
        _______, CK(CH_TILD), CK(CH_LBRC), CK(CH_RBRC), CK(CH_UNDS), CK(CH_HASH),      CK(CH_PIPE), CK(CH_EXLM), CK(CH_SCLN), CK(CH_COLN), CK(CH_QUES), _______,
                                                        _______,     _______, _______,   _______, _______, _______
    ),
    // Navigation (gauche) + pavé numérique (droite)
    // Pouce Sym transparent : Nav puis Sym active _ADJ, quel que soit l'ordre
    [_NAV] = LAYOUT(
        _______, KC_F1,         KC_F2,       KC_F3,       KC_F4,        KC_F5,          KC_F6,       KC_F7,     KC_F8,     KC_F9,     KC_F10,      KC_F11,
        _______, KC_ESC,        KC_HOME,     KC_UP,       KC_END,       KC_PGUP,        CK(CH_SLSH), CK(CH_7),  CK(CH_8),  CK(CH_9),  CK(CH_ASTR), KC_F12,
        _______, CK(CH_SELALL), KC_LEFT,     KC_DOWN,     KC_RGHT,      KC_PGDN,        CK(CH_MINS), CK(CH_4),  CK(CH_5),  CK(CH_6),  CK(CH_0),    KC_PSCR,
        _______, CK(CH_UNDO),   CK(CH_CUT),  CK(CH_COPY), CK(CH_PASTE), CK(CH_REDO),    CK(CH_COMM), CK(CH_1),  CK(CH_2),  CK(CH_3),  CK(CH_DOT),  CK(CH_PLUS),
                                                          _______,      _______, _______,   KC_DEL,  _______,     _______
    ),
    // Nav + Sym : réglages. Hôte sur les lettres Ergo-L W (Windows), L (Linux), M (macOS)
    [_ADJ] = LAYOUT(
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, HST_WIN,                   XXXXXXX, HST_MAC, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,                   HST_LNX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
                                             _______, _______, _______,   _______, _______, _______
    ),
};
// clang-format on

/******************************************************************************
 * Hôte (sauvegardé en EEPROM)
 ******************************************************************************/

typedef union {
    uint32_t raw;
    struct {
        uint8_t host : 2;
    };
} user_config_t;

static user_config_t user_config;

void eeconfig_init_user(void) {
    user_config.raw = 0;
    eeconfig_update_user(user_config.raw);
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    if (user_config.host >= HOST_COUNT) user_config.host = HOST_WIN;
}

static void set_host(uint8_t host) {
    user_config.host = host;
    eeconfig_update_user(user_config.raw);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _NAV, _SYM, _ADJ);
}

/******************************************************************************
 * Envoi des caractères
 ******************************************************************************/

// Code Alt Windows « Alt+0nnn » sur le pavé numérique (Verr Num doit être actif).
static void send_altcode(uint8_t code) {
    static const uint8_t keypad[] = {KC_P0, KC_P1, KC_P2, KC_P3, KC_P4, KC_P5, KC_P6, KC_P7, KC_P8, KC_P9};

    const bool numlock_off = !host_keyboard_led_state().num_lock;
    if (numlock_off) tap_code(KC_NUM_LOCK);

    register_code(KC_LALT);
    tap_code(KC_P0);
    tap_code(keypad[code / 100]);
    tap_code(keypad[code / 10 % 10]);
    tap_code(keypad[code % 10]);
    unregister_code(KC_LALT);

    if (numlock_off) tap_code(KC_NUM_LOCK);
}

// Saisie Unicode IBus/GTK : Ctrl+Maj+U, code hexa, espace.
// Les chiffres hexa sont tapés en AZERTY (le mode Unicode de QMK suppose un QWERTY).
static void send_unicode_linux(uint16_t cp) {
    static const uint16_t hex[] = {FR_0, FR_1, FR_2, FR_3, FR_4, FR_5, FR_6, FR_7,
                                   FR_8, FR_9, FR_A, FR_B, FR_C, FR_D, FR_E, FR_F};

    tap_code16(C(S(FR_U)));
    for (int8_t shift = 12; shift >= 0; shift -= 4) {
        tap_code16(hex[(cp >> shift) & 0xF]);
    }
    tap_code(KC_SPC);
}

// Majuscule accentuée via Verr Maj (Linux, macOS : Verr Maj + é = É)
static void send_with_caps(uint16_t kc) {
    const bool caps_off = !host_keyboard_led_state().caps_lock;
    if (caps_off) tap_code(KC_CAPS);
    tap_code16(kc);
    if (caps_off) tap_code(KC_CAPS);
}

static void tap_output(const out_t *out) {
    const uint8_t mods = get_mods();
    clear_mods();
    clear_weak_mods();
    send_keyboard_report();

    switch (out->type) {
        case O_KEY:
            tap_code16(out->a);
            break;
        case O_SEQ:
            tap_code16(out->a);
            tap_code16(out->b);
            break;
        case O_CAPS:
            send_with_caps(out->a);
            break;
        case O_ALT:
            send_altcode(out->a);
            break;
        case O_UNI:
            send_unicode_linux(out->a);
            break;
    }

    set_mods(mods);
    send_keyboard_report();
}

/******************************************************************************
 * Touche tenue et répétition
 *
 * Une sortie O_KEY reste enfoncée côté hôte tant que la touche est tenue :
 * c'est l'OS qui répète. Les séquences (touche morte, code Alt, Unicode) sont
 * rejouées par le firmware.
 ******************************************************************************/

#define REPEAT_DELAY 500
#define REPEAT_INTERVAL 80

static uint16_t       held_keycode    = KC_NO; // keycode custom tenu
static uint16_t       held_host_key   = KC_NO; // touche hôte enfoncée (O_KEY)
static uint8_t        suppressed_mods = 0;     // Maj retiré pendant l'appui
static const out_t   *repeat_out      = NULL;
static deferred_token repeat_token    = INVALID_DEFERRED_TOKEN;

static uint32_t repeat_callback(uint32_t trigger_time, void *cb_arg) {
    tap_output(repeat_out);
    return REPEAT_INTERVAL;
}

static void release_held(void) {
    if (held_host_key != KC_NO) {
        unregister_code16(held_host_key);
        add_mods(suppressed_mods);
        send_keyboard_report();
    }
    if (repeat_token != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec(repeat_token);
    }
    held_keycode    = KC_NO;
    held_host_key   = KC_NO;
    suppressed_mods = 0;
    repeat_token    = INVALID_DEFERRED_TOKEN;
}

static void press_output(uint16_t keycode, const out_t *out, bool raw) {
    held_keycode = keycode;

    if (raw) {
        // Raccourci (Ctrl+1, Cmd+-…) : touche brute avec les modificateurs tenus
        held_host_key = QK_MODS_GET_BASIC_KEYCODE(out->a);
        register_code(held_host_key);
    } else if (out->type == O_KEY) {
        suppressed_mods = get_mods() & MOD_MASK_SHIFT;
        del_mods(suppressed_mods);
        clear_weak_mods();
        held_host_key = out->a;
        register_code16(held_host_key);
    } else {
        tap_output(out);
        repeat_out   = out;
        repeat_token = defer_exec(REPEAT_DELAY, repeat_callback, NULL);
    }
}

/******************************************************************************
 * Traitement des touches
 ******************************************************************************/

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    const bool pressed = record->event.pressed;

    if (held_keycode != KC_NO) {
        if (pressed) {
            release_held(); // une autre touche prend le relais (roulement)
        } else if (keycode == held_keycode) {
            release_held();
            return false;
        } else if (keycode == KC_LSFT || keycode == KC_RSFT) {
            suppressed_mods &= ~MOD_BIT(keycode); // Maj relâché pendant l'appui : ne pas le rétablir
        }
    }

    const uint8_t mods  = get_mods() | get_oneshot_mods();
    const bool    shift = mods & MOD_MASK_SHIFT;

    switch (keycode) {
        case ERG_ODK:
            if (pressed) {
                if (shift) {
                    clear_oneshot_mods();
                    press_output(keycode, &chars[CH_EXLM][user_config.host], false);
                } else {
                    set_oneshot_layer(_ODK, ONESHOT_START);
                }
            } else {
                clear_oneshot_layer_state(ONESHOT_PRESSED);
            }
            return false;
        case HST_WIN:
        case HST_LNX:
        case HST_MAC:
            if (pressed) set_host(keycode - HST_WIN);
            return false;
    }

    uint8_t ch, lower;
    if (keycode >= ERG_FIRST && keycode < ERG_LAST) {
        lower = erg_keys[keycode - ERG_FIRST][0];
        ch    = erg_keys[keycode - ERG_FIRST][shift];
    } else if (keycode >= CHAR_FIRST && keycode < CHAR_LAST) {
        lower = ch = keycode - CHAR_FIRST;
    } else {
        return true;
    }
    if (!pressed) return false;

    // Ces touches ne passent pas par process_action : on désactive la couche 1dk à la main.
    if (is_oneshot_layer_active()) clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
    clear_oneshot_mods();

    const out_t *lower_out = &chars[lower][user_config.host];
    if ((mods & ~MOD_MASK_SHIFT) && lower_out->type == O_KEY) {
        press_output(keycode, lower_out, true);
    } else {
        press_output(keycode, &chars[ch][user_config.host], false);
    }
    return false;
}
