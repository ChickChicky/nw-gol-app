#ifndef PALETTE_H
#define PALETTE_H

#include "common.h"

#define MAKE_PALETTE(colors)(pal_t){.c##olors=(colors),.size=sizeof(colors)/sizeof(eadk_color_t)}

#define RGB(r,g,b)((((b)*31/255)<<0)|(((g)*63/255)<<5)|(((r)*31/255)<<11))

typedef struct pal {
  eadk_color_t* colors;
  uint32_t size;
} pal_t;

pal_t pal_custom = MAKE_PALETTE(((eadk_color_t[]){
  0x0000,
  0x0841,
  0x1082,
  0x18c3,
  0x2104,
  0x2945,
  0x3186,
  0x39c7,
  0x4208,
  0x4a49,
  0x528a,
  0x5acb,
  0x630c,
  0x6b4d,
  0x738e,
  0x7bcf,
  0x8410,
  0x8c51,
  0x9492,
  0x9cd3,
  0xa514,
  0xad55,
  0xb596,
  0xbdd7,
  0xc618,
  0xce59,
  0xd69a,
  0xdedb,
  0xe71c,
  0xef5d,
  0xf79e,
  0xffff,
}));

pal_t pal_default = MAKE_PALETTE(((eadk_color_t[]){
  0x0000,
  0xffff,
}));

pal_t pal_cool = MAKE_PALETTE(((eadk_color_t[]){
  0x0000,
  0xffe0,
  0xf800,
}));

pal_t pal_crystal = MAKE_PALETTE(((eadk_color_t[]){
  RGB(0,   0,   0  ),
  RGB(175, 186, 247),
  RGB(162, 162, 223),
  RGB(150, 139, 200),
  RGB(138, 115, 177),
  RGB(126, 92,  153),
  RGB(114, 68,  130),
  RGB(102, 45,  107),
}));

#endif