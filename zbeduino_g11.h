#pragma once

#if 0
Byte1 -> 11 -> Action Counter
Byte2 -> FD -> Rotary position
Byte3 -> 7F -> Rotary position
Byte4 -> 00 -> Rotary Angle (10 = UP, 40 RIGHT...)
Byte5 -> 20 -> Back, 04, MENU
Byte6 -> 01 -> Option, 08 COM
Byte7 -> C1 -> Media C8, NAV
Byte8 -> F9 -> Map,
#endif

#define BYTE_ACTION_COUNTER 0
#define BYTE_ROTARY_POS1 1
#define BYTE_ROTARY_POS2 2
#define BYTE_ROTARY_ANGLE 3
#define BYTE_BUTTON_SET1 4
#define BYTE_BUTTON_SET2 5
#define BYTE_BUTTON_SET3 6
#define BYTE_BUTTON_SET4 7

#define ZBE_MAP 0x1
//#define ZBE_MAP      0b0001
#define ZBE_NAV      0b100000000000
#define ZBE_COM      0b000010000000000000000000
#define ZBE_MEDIA    0b000000000000100000000000
#define ZBE_MENU     0b00000100000000000000000000000000
#define ZBE_BACK     0b00100000000000000000000000000000
#define ZBE_OPTION   0b00000000000000010000000000000000
#define ZBE_ENTER    0b100000 // FIXME
