#ifndef CONFIGS_H
#define CONFIGS_H

#include "common.h"
#include "palettes.h"

typedef struct config {
  float    F; // How much should the grid be filled when initializing
  uint16_t B; // Birth rule
  uint16_t S; // Survival rule
  uint8_t  s; // (WIP) State count
  const char* name; // The name of the rule / configuration
  pal_t* palette; // The color palette of the rule
} config_t;

static const char* const menu_custom_name = "[Custom]";
static const char* const menu_config_name = "[Configure]";

#define NCONFIGS ((sizeof(configs)/sizeof(config_t))-1)

config_t configs[] = { // All of the preset configurations
  { .F = .50f,
    .B = 0b000001000,
    .S = 0b000001100,
    .s = 2,
    .name = menu_custom_name,
    .palette = &pal_custom,
  },
  { .name = menu_config_name, },
  { .F = .50f,
    .B = 0b000001000,
    .S = 0b000001100,
    .s = 2,
    .name = "GOL",
    .palette = &pal_default,
  },
  { .F = .75f,
    .B = 0b000001000,
    .S = 0b000111110,
    .s = 2,
    .name = "Maze",
    .palette = &pal_default,
  },
  { .F = .75f,
    .B = 0b011000100,
    .S = 0b111101100,
    .s = 2,
    .name = "Coagulation",
    .palette = &pal_default,
  },
  { .F = .50f,
    .B = 0b010001000,
    .S = 0b100111000,
    .s = 3,
    .name = "Cool",
    .palette = &pal_cool,
  },
  { .F = .30f /*.10f*/,
    .B = 0b000000100,
    .S = 0b000010100,
    .s = 8,
    .name = "Crystal",
    .palette = &pal_crystal,
  },
  { .F = .30f /*.10f*/,
    .B = 0b000101010,
    .S = 0b110111100,
    .s = 2,
    .name = "Land Rush",
    .palette = &pal_default,
  },
  { .F = .40f /*.30f*/,
    .B = 0b010000010,
    .S = 0b000000110,
    .s = 5,
    .name = "Bricks",
    .palette = &pal_default,
  },
  { .F = .30f /*.05f*/,
    .B = 0b011000100,
    .S = 0b001111000,
    .s = 17,
    .name = "Cyber",
    .palette = &pal_cyber,
  },
  { .F = .30f /*.10f*/,
    .B = 0b000111100,
    .S = 0b001111000,
    .s = 8,
    .name = "Circuit Genesis",
    .palette = &pal_circgen,
  },
  { .F = .40f /*.01f*/,
    .B = 0b010001000,
    .S = 0b101010111,
    .s = 18,
    .name = "Ebb & Flow II",
    .palette = &pal_circgen,
  },
  { .F = .90f,
    .B = 0b100110000,
    .S = 0B000111111,
    .s = 3,
    .name = "Sticks",
    .palette = &pal_custom,
  },
  { 0 }
};

#endif