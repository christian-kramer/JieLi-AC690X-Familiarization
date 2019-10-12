
#include "rcsp/light_control/light_fade.h"
#include "rcsp/light_control/light_pwm.h"
#include <string.h>

#if SUPPORT_APP_RCSP_EN

typedef struct _light_fade
{
    //闪烁当前计数值
    volatile u16 cur_w;
    volatile u16 cur_r;
    volatile u16 cur_g;
    volatile u16 cur_b;

    //闪烁目标值
    volatile u16 tag_w;
    volatile u16 tag_r;
    volatile u16 tag_g;
    volatile u16 tag_b;

    //闪烁标记
    volatile u8  flag_w; 
    volatile u8  flag_r;
    volatile u8  flag_g;
    volatile u8  flag_b;

} light_fade_t;

light_fade_t led_fade_var;

void led_fade_init(void)
{
    memset(&led_fade_var,0,sizeof(light_fade_t));
}

tbool led_is_fading(void)
{
    
//    P_FUNCTION
//    printf("fadeing= %d \n",led_fade_var.flag_r | led_fade_var.flag_g | led_fade_var.flag_b | led_fade_var.flag_w);

    return  led_fade_var.flag_r | led_fade_var.flag_g | led_fade_var.flag_b | led_fade_var.flag_w;
}

void led_fade(void)
{
//    P_FUNCTION
//    printf("fade= %d \n",led_fade_var.flag_r | led_fade_var.flag_g | led_fade_var.flag_b | led_fade_var.flag_w);

    //white
    if (led_fade_var.flag_w)
    {
        if (led_fade_var.cur_w > led_fade_var.tag_w)
        {
            led_fade_var.cur_w--;
        }
        else if (led_fade_var.cur_w < led_fade_var.tag_w)
        {
            led_fade_var.cur_w++;
        }
        PWM_output_cfg(OUT_CH1_SEL,1,led_fade_var.cur_w,FICKER_FREQUENCY);
        if (led_fade_var.cur_w == led_fade_var.tag_w)
        {
            led_fade_var.flag_w = 0;
        }
        else if (led_fade_var.flag_w>1)
        {
            led_fade_var.flag_w = 0;
        }
    }
    
    //red
    if (led_fade_var.flag_r)
    {
        if (led_fade_var.cur_r > led_fade_var.tag_r)
        {
            led_fade_var.cur_r--;
        }
        else if (led_fade_var.cur_r < led_fade_var.tag_r)
        {
            led_fade_var.cur_r++;
        }
        PWM_output_cfg(OUT_ASSIGN_PORT,1,led_fade_var.cur_r,FICKER_FREQUENCY);       //控制红灯的PWM
        if (led_fade_var.cur_r == led_fade_var.tag_r)
        {
            led_fade_var.flag_r = 0;
        }
        else if (led_fade_var.flag_r>1)
        {
            led_fade_var.flag_r = 0;
        }
    }
    
    //green
    if (led_fade_var.flag_g)
    {
        if (led_fade_var.cur_g > led_fade_var.tag_g)
        {
            led_fade_var.cur_g--;
        }
        else if (led_fade_var.cur_g < led_fade_var.tag_g)
        {
            led_fade_var.cur_g++;
        }
        PWM_output_cfg(OUT_CH1_SEL,1,led_fade_var.cur_g,FICKER_FREQUENCY);   //控制绿灯的PWM
        if (led_fade_var.cur_g == led_fade_var.tag_g)
        {
            led_fade_var.flag_g = 0;
        }
        else if (led_fade_var.flag_g>1)
        {
            led_fade_var.flag_g = 0;
        }
    }
    
    //blue
    if (led_fade_var.flag_b)
    {
        if (led_fade_var.cur_b > led_fade_var.tag_b)
        {
            led_fade_var.cur_b--;
        }
        else if (led_fade_var.cur_b < led_fade_var.tag_b)
        {
            led_fade_var.cur_b++;
        }
        PWM_output_cfg(OUT_CH0_SEL,2,led_fade_var.cur_b,FICKER_FREQUENCY);   //控制蓝灯的PWM
        if (led_fade_var.cur_b == led_fade_var.tag_b)
        {
            led_fade_var.flag_b = 0;
        }
        else if (led_fade_var.flag_b>1)
        {
            led_fade_var.flag_b = 0;
        }
    }
}

void led_w_pwm_ctrl(u16 duty, tbool fade)
{
//    P_FUNCTION
//    printf("duty= %d,fade= %d \n",duty,fade);
    
//    put_buf((void*)&led_fade_var,sizeof(light_fade_t));

#if 1
    if (led_fade_var.flag_w)
    {
        led_fade_var.flag_w++;
        while(led_fade_var.flag_w);
    }
    if (fade)
    {
        led_fade_var.tag_w = duty;
        led_fade_var.flag_w = 1;
//        while(led_fade_var.flag_w);
    }
    else
    {
        led_fade_var.cur_w = led_fade_var.tag_w = duty;
        PWM_output_cfg(OUT_CH1_SEL,1,led_fade_var.cur_w,FICKER_FREQUENCY);
    }
#endif
}

void led_r_pwm_ctrl(u16 duty, tbool fade)
{
//    P_FUNCTION
//    printf("duty= %d,fade= %d \n",duty,fade);

#if 1
    if (led_fade_var.flag_r)
    {
        led_fade_var.flag_r++;
        while(led_fade_var.flag_r);
    }
    
    if (fade)
    {
        led_fade_var.tag_r = duty;
        led_fade_var.flag_r = 1;
//        while(led_fade_var.flag_r);
    }
    else
    {
        led_fade_var.cur_r = led_fade_var.tag_r = duty;
        PWM_output_cfg(OUT_ASSIGN_PORT,1,led_fade_var.cur_r,FICKER_FREQUENCY);       //控制红灯的PWM
    }    
#endif

}

void led_g_pwm_ctrl(u16 duty, tbool fade)
{
//    P_FUNCTION
//    printf("duty= %d,fade= %d \n",duty,fade);

#if 1
    if (led_fade_var.flag_g)
    {
        led_fade_var.flag_g++;
        while(led_fade_var.flag_g);
    }
    
    if (fade)
    {
        led_fade_var.tag_g = duty;
        led_fade_var.flag_g = 1;
//        while(led_fade_var.flag_g);
    }
    else
    {
        led_fade_var.cur_g = led_fade_var.tag_g = duty;
        PWM_output_cfg(OUT_CH1_SEL,1,led_fade_var.cur_g,FICKER_FREQUENCY);   //控制绿灯的PWM
    }
#endif

}

void led_b_pwm_ctrl(u16 duty, tbool fade)
{

//    P_FUNCTION
//    printf("duty= %d,fade= %d \n",duty,fade);    
    
#if 1
    if (led_fade_var.flag_b)
    {
        led_fade_var.flag_b++;
        while(led_fade_var.flag_b);
    }
    
    if (fade)
    {
        led_fade_var.tag_b = duty;
        led_fade_var.flag_b = 1;
//        while(led_fade_var.flag_b);
    }
    else
    {
        led_fade_var.cur_b = led_fade_var.tag_b = duty;
        PWM_output_cfg(OUT_CH0_SEL,2,led_fade_var.cur_b,FICKER_FREQUENCY);   //控制蓝灯的PWM
    }
#endif

}


#endif

