#include "common.h"
#include "configs.h"
#include "palettes.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "GOL+";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

// Waits for the specified key to be released (eadk_key_*)
#define WAIT_RELEASE(k){while((eadk_keyboard_key_down(eadk_keyboard_scan(),k)))eadk_timing_msleep(10);}

#define FLAG_PAUSED 0b00000000000000000000000000000001
#define FLAG_STEP   0b00000000000000000000000000000010

#define STATE_MENU   0
#define STATE_SIM    1
#define STATE_CUSTOM 2
#define STATE_CONFIG 3

// The amount of time between scrolls while holding keys
#define HOLD_PAUSE 150

pal_t* palette;

uint8_t board[240*240] = {0}; // Board
uint8_t bboard[240*240] = {0}; // Backboard

uint16_t B; // Birth Rule
uint16_t S; // Survival Rule
uint8_t  s; // State Count
float    F; // Fill Rate

config_t conf; // Currently Selected Configuration
uint32_t flags; // Simulation Flags

uint8_t selected; // Current Configuration Selection Index
uint64_t lt; // Last Time Since Last Key Hold Update
// TODO: Implement multiple key hold timers

uint8_t ctm_locc; // Custom Configuration Selected Column
uint8_t ctm_locr; // Custom Configuration Selected Row

uint8_t cfg_locc; // Configuration Selected Column
uint8_t cfg_locr; // Configuration Selected Row

uint8_t state; // Application State

uint32_t w;
uint32_t h;
uint32_t sz;

uint8_t brightness;

char tmp[64] = {0};

static inline float clampf(float v, float n, float m) {
  v = (v<n)?n:v;
  return (v>m)?m:v;
}

static inline int clampi(int v, int n, int m) {
  v = (v<n)?n:v;
  return (v>m)?m:v;
}

static inline eadk_color_t palette_get(uint8_t s, uint8_t t) {
  if (!palette) return 0xF81F | (((uint32_t)s)*31/255);
  return palette->colors[((uint32_t)t)*(palette->size-1)/(((uint32_t)s)-1ull)];
}

static inline void sim_refresh(void) {
  for (size_t x = 0; x < w; x++) for (size_t y = 0; y < h; y++) {
    eadk_display_push_rect_uniform((eadk_rect_t){x,y,1,1},palette_get(s,board[x+y*w]));
  }
}

static inline void update_brightness(void) {
  eadk_backlight_set_brightness(brightness);
  float bw = 64.f;
  float bright = ((float)brightness)/255.f;
  eadk_display_push_rect_uniform((eadk_rect_t){320-8-((int)bw),240-16-4,(int)floorf(bw*(1-bright)),4},eadk_color_black);
  eadk_display_push_rect_uniform((eadk_rect_t){320-8-((int)bw)+(int)floorf(bw*(1-bright)),240-16-4,(int)ceilf(bw*bright),4},eadk_color_blue);
  eadk_display_push_rect_uniform((eadk_rect_t){320-8-((int)bw)-1,240-16-4-1,1,6},eadk_color_white);
  eadk_display_push_rect_uniform((eadk_rect_t){320-8,240-16-4-1,1,6},eadk_color_white);
}

static inline void init_menu(bool first) { state = STATE_MENU;
  if (!first) return;
  selected = 2;
  lt = 0;
}

static inline void init_sim(bool first) { state = STATE_SIM;
  if (first) return;
  conf = configs[selected];
  flags = 0;
  B = conf.B;
  S = conf.S;
  s = conf.s;
  F = conf.F;
  palette = conf.palette;
  const uint32_t fillrate = UINT32_MAX*F;
  for (size_t i = 0; i < sz; i++)
    board[i] = (eadk_random() < fillrate)*(eadk_random()%s);
}

static inline void init_custom(bool first) { state = STATE_CUSTOM;
  if (!first) return;
  ctm_locc = 0;
  ctm_locr = 0;
}

static inline void init_config(bool first) { state = STATE_CONFIG;
  if (first) {
    cfg_locc = 0;
    cfg_locr = 0;
    w = 240;
    h = 240;
  }
  sz = w*h;
}

int main(int argc, char* argv[]) {
  //// Configuration Selection Menu ////
  
  WAIT_RELEASE(eadk_key_ok);

  eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);

  init_menu(true);
  init_sim(true);
  init_custom(true);
  init_config(true);

  brightness = eadk_backlight_brightness();

  state = STATE_MENU;

  for (uint32_t frame=0;1;frame++) {
    eadk_keyboard_state_t kbd = eadk_keyboard_scan();
    
    if (eadk_keyboard_key_down(kbd,eadk_key_plus)) {
      brightness = clampi(((int)brightness)+25,0,255);
      update_brightness();
    }
    else if (eadk_keyboard_key_down(kbd,eadk_key_minus)) {
      brightness = clampi(((int)brightness)-25,0,255);
      update_brightness();
    }
    else if (!(frame%16)) {
      update_brightness();
    }

    
    uint64_t t = eadk_timing_millis();

    if (state == STATE_MENU) {
      if (eadk_keyboard_key_down(kbd,eadk_key_up)) {
        if (t-lt > HOLD_PAUSE) {
          selected = MOD(((int16_t)selected)-1,NCONFIGS);
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_down)) {
        if (t-lt > HOLD_PAUSE) {
          selected = MOD(selected+1,NCONFIGS);
          lt = t;
        }
      }
      else {
        lt = 0;
      }
      
      if (eadk_keyboard_key_down(kbd,eadk_key_ok)) {
        if (configs[selected].name == menu_custom_name) {
          init_custom(false);
        } else if (configs[selected].name == menu_config_name) {
          init_config(false);
        } else {
          init_sim(false);
        }
        WAIT_RELEASE(eadk_key_ok);
        eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);
        if (state == STATE_SIM) sim_refresh();
        continue;
      }
      
      if (eadk_keyboard_key_down(kbd,eadk_key_back)) {
        WAIT_RELEASE(eadk_key_back);
        eadk_timing_msleep(100);
        return 0;
      }
      
      for (size_t i = 0; i < NCONFIGS; i++) {
        config_t* conf = &configs[i];
        eadk_display_draw_string(conf->name,(eadk_point_t){1,1+i*14},false,i==selected?((conf->name==menu_custom_name||conf->name==menu_config_name)?(eadk_color_red|eadk_color_green):eadk_color_green):eadk_color_white,eadk_color_black);
      }
    }

    else if (state == STATE_CUSTOM) {
      if (eadk_keyboard_key_down(kbd,eadk_key_up)) {
        if (t-lt > HOLD_PAUSE) {
          ctm_locr = MOD(((int16_t)ctm_locr)-1,5);
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_down)) {
        if (t-lt > HOLD_PAUSE) {
          ctm_locr = MOD(ctm_locr+1,5);
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_left)) {
        if (t-lt > HOLD_PAUSE) {
          if (ctm_locr == 0 || ctm_locr == 1) {
            ctm_locc = MOD(ctm_locc-1,9);
          } else if (ctm_locr == 2) {
            configs[selected].s = clampi(configs[selected].s-1,2,pal_custom.size-1);
          } else if (ctm_locr == 3) {
            configs[selected].F = ((float)((int)(clampf(configs[selected].F-.1f,0.f,1.f)*10.f)))/10.f;
          } else if (ctm_locr == 4) {

          }
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_right)) {
        if (t-lt > HOLD_PAUSE) {
          if (ctm_locr == 0 || ctm_locr == 1) {
            ctm_locc = MOD(ctm_locc+1,9);
          } else if (ctm_locr == 2) {
            configs[selected].s = clampi(configs[selected].s+1,2,pal_custom.size-1);
          } else if (ctm_locr == 3) {
            configs[selected].F = ((float)((int)(clampf(configs[selected].F+.1f,0.f,1.f)*10.f)))/10.f;
          } else if (ctm_locr == 4) {

          }
          lt = t;
        }
      }
      else {
        lt = 0;
      }

      if (eadk_keyboard_key_down(kbd,eadk_key_back)) {
        init_menu(false);
        WAIT_RELEASE(eadk_key_back);
        eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);
        continue;
      }

      if (eadk_keyboard_key_down(kbd,eadk_key_ok)) {
        if (ctm_locr == 0) {
          configs[selected].B ^= (1<<ctm_locc);
          WAIT_RELEASE(eadk_key_ok);
        }
        else if (ctm_locr == 1) {
          configs[selected].S ^= (1<<ctm_locc);
          WAIT_RELEASE(eadk_key_ok);
        }
        else if (ctm_locr == 2) {
          
        }
        else if (ctm_locr == 3) {
          
        }
        else if (ctm_locr == 4) {
          init_sim(false);
          WAIT_RELEASE(eadk_key_ok);
          eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);
          sim_refresh();
          continue;
        }
      }

      eadk_display_draw_string("Birth",(eadk_point_t){1,1+14*0},false,ctm_locr==0?eadk_color_green:eadk_color_white,eadk_color_black);
      eadk_display_draw_string("Survival",(eadk_point_t){1,1+14*1},false,ctm_locr==1?eadk_color_green:eadk_color_white,eadk_color_black);
      eadk_display_draw_string("States",(eadk_point_t){1,1+14*2},false,ctm_locr==2?eadk_color_green:eadk_color_white,eadk_color_black);
      eadk_display_draw_string("Fill Rate",(eadk_point_t){1,1+14*3},false,ctm_locr==3?eadk_color_green:eadk_color_white,eadk_color_black);
      eadk_display_draw_string("Run!",(eadk_point_t){1,1+14*4},false,ctm_locr==4?eadk_color_green:eadk_color_white,eadk_color_black);

      for (size_t i = 0; i < 9; i++) {
        tmp[0] = '0'+i; tmp[1] = 0;
        eadk_display_draw_string(tmp,(eadk_point_t){1+8*9+i*10,1+14*0},false,(ctm_locr==0&&ctm_locc==i)?eadk_color_green:(configs[selected].B&(1<<i)?eadk_color_black:eadk_color_white),configs[selected].B&(1<<i)?eadk_color_white:eadk_color_black);
      }
      for (size_t i = 0; i < 9; i++) {
        tmp[0] = '0'+i; tmp[1] = 0;
        eadk_display_draw_string(tmp,(eadk_point_t){1+8*9+i*10,1+14*1},false,(ctm_locr==1&&ctm_locc==i)?eadk_color_green:(configs[selected].S&(1<<i)?eadk_color_black:eadk_color_white),configs[selected].S&(1<<i)?eadk_color_white:eadk_color_black);
      }
      sprintf(tmp,"%d   ",(int)(configs[selected].s));
      eadk_display_draw_string(tmp,(eadk_point_t){1+8*9,1+14*2},false,ctm_locr==2?eadk_color_green:eadk_color_white,eadk_color_black);
      sprintf(tmp,"%d%% ",(int)(configs[selected].F*100.f));
      eadk_display_draw_string(tmp,(eadk_point_t){1+8*9,1+14*3},false,ctm_locr==3?eadk_color_green:eadk_color_white,eadk_color_black);
    }

    else if (state == STATE_CONFIG) {
      if (eadk_keyboard_key_down(kbd,eadk_key_up)) {
        if (t-lt > HOLD_PAUSE) {
          cfg_locr = MOD(((int16_t)cfg_locr)-1,2);
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_down)) {
        if (t-lt > HOLD_PAUSE) {
          cfg_locr = MOD(cfg_locr+1,2);
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_left)) {
        if (t-lt > HOLD_PAUSE) {
          if (cfg_locr == 0) {
            w = clampi(w-10,10,240);
          }
          else if (cfg_locr == 1) {
            h = clampi(h-10,10,240);
          }
          lt = t;
        }
      }
      else if (eadk_keyboard_key_down(kbd,eadk_key_right)) {
        if (t-lt > HOLD_PAUSE) {
          if (cfg_locr == 0) {
            w = clampi(w+10,10,240);
          }
          else if (cfg_locr == 1) {
            h = clampi(h+10,10,240);
          }
          lt = t;
        }
      }
      else {
        lt = 0;
      }

      if (eadk_keyboard_key_down(kbd,eadk_key_back)) {
        init_menu(false);
        WAIT_RELEASE(eadk_key_back);
        eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);
        continue;
      }

      eadk_display_draw_string("Width",(eadk_point_t){1,1+14*0},false,cfg_locr==0?eadk_color_green:eadk_color_white,eadk_color_black);
      eadk_display_draw_string("Height",(eadk_point_t){1,1+14*1},false,cfg_locr==1?eadk_color_green:eadk_color_white,eadk_color_black);

      sprintf(tmp,"%d   ",(int)(w));
      eadk_display_draw_string(tmp,(eadk_point_t){1+8*7,1+14*0},false,cfg_locr==0?eadk_color_green:eadk_color_white,eadk_color_black);
      sprintf(tmp,"%d   ",(int)(h));
      eadk_display_draw_string(tmp,(eadk_point_t){1+8*7,1+14*1},false,cfg_locr==1?eadk_color_green:eadk_color_white,eadk_color_black);

      sz = w*h;
    }

    else if (state == STATE_SIM) {
      // back -> menu
      if (eadk_keyboard_key_down(kbd,eadk_key_back)) {
        WAIT_RELEASE(eadk_key_back);
        eadk_display_push_rect_uniform(eadk_screen_rect,eadk_color_black);
        continue;
      }

      // ok -> step once (+ pause)
      if (eadk_keyboard_key_down(kbd,eadk_key_ok)) {
        if (t-lt > HOLD_PAUSE) {
          flags |= FLAG_PAUSED | FLAG_STEP;
        }
      }
      // right -> pause
      else if (eadk_keyboard_key_down(kbd,eadk_key_right)) {
        if (t-lt > HOLD_PAUSE) {
          flags &= ~FLAG_PAUSED;
        }
      }
      // left -> unpause
      else if (eadk_keyboard_key_down(kbd,eadk_key_left)) {
        if (t-lt > HOLD_PAUSE) {
          flags |= FLAG_PAUSED;
        }
      }
      else {
        lt = t;
      }
    
      for (int32_t x = 0; x < w; x++) for (int32_t y = 0; y < h; y++) {
        size_t i = x+y*w;

        if (!(flags&FLAG_PAUSED)||(flags&FLAG_STEP)) {
          uint8_t u = board[i];
          uint8_t v;

          if (u>0 && u<s-1) {
            v = bboard[i] = bboard[i]-1;
          } else {
            uint8_t n = 0;
            for (int32_t dx = -1; dx < 2; dx++) {
              for (int32_t dy = -1; dy < 2; dy++) {
                if (dx == 0 && dy == 0) continue;
                n += board[MOD(x+dx,(signed)w)+MOD(y+dy,(signed)h)*w]==(s-1);
              }
            }
            uint8_t k = u ? (S&(1<<n)) : (B&(1<<n));
            bboard[i] = v = k ? (s-1) : (u==0 ? 0 : s-2);
          }

          if (!(frame%128) || u != v) // Full screen refresh every 128 frames
            eadk_display_push_rect_uniform((eadk_rect_t){x,y,1,1},palette_get(s,v));
        }
      }
      flags &= ~FLAG_STEP;
  
      memcpy(board,bboard,sz);
    }

    eadk_display_wait_for_vblank();
  }

}
