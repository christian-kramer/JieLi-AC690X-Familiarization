
#ifndef _LIGHT_FADE_H
#define _LIGHT_FADE_H

#include "light_def.h"

#define LED_FADE_ON     1
#define LED_FADE_OFF    0

void led_fade_init(void);
tbool led_is_fading(void);
void led_fade(void);
void led_w_pwm_ctrl(u16 duty, tbool fade);
void led_r_pwm_ctrl(u16 duty, tbool fade);
void led_g_pwm_ctrl(u16 duty, tbool fade);
void led_b_pwm_ctrl(u16 duty, tbool fade);

#endif
