#include "rcsp/light_control/light_def.h"
#include "rcsp/light_control/light_pwm.h"
#include "rcsp/light_control/light_fade.h"

#if SUPPORT_APP_RCSP_EN

#define ENERGY_GAIN  15

LIGHT_ATTRIBUTE light_attr;      //灯属性
u8 light_on_off = 1;             //灯开关
u8 flicker_effect;               //灯闪烁模式,0:LED_FLASH_100MS  1:LED_FLASH_300MS  2:LED_FLASH_500MS
u8 colorful_flag = 0;            //七色显示彩色标志
u8 led_colour_cnt = 0;
u8 music_colour_cnt = 0;
u8 set_brightness_flag =0;       //设置亮度标志

volatile u8 time_50ms_flag =0;   //50ms标志
volatile u8 time_100ms_flag =0;  //100ms标志
volatile u8 time_300ms_flag =0;  //300ms标志
volatile u8 time_500ms_flag =0;  //500ms标志

static u8 led_init_flag = 0;

//light_disp_t  light_scene_control[LGT_SCENE_UNIT_MAX];
//
//u16 light_scene_target[LGT_SCENE_UNIT_MAX];
//u16 light_scene_cur[LGT_SCENE_UNIT_MAX];
////50MS步进
//u8 light_scene_step[LGT_SCENE_UNIT_MAX];

//malloc data
light_disp_t  *light_scene_control = NULL;
u16 *light_scene_target ;
u16 *light_scene_cur;
//50MS步进
u8 *light_scene_step;


u16 light_scene_10ms_cnt;
u16 light_scene_s_cnt;
u16 light_scene_cycle;

//extern volatile u32 dac_energy_value;


/*七彩颜色的RGB表*/

const u8 colorful[7][3] =
{
    {0xFF, 0   , 0   },  //红
    {0xFF, 0x61, 0   },  //橙
    {0xFF, 0xFF, 0   },  //黄
    {0   , 0xFF, 0   },  //绿
    {0x7C, 0xFC, 0   },  //青
    {0   , 0   , 0xFF},  //蓝
    {0xA0, 0x20, 0xF0},  //紫
};

/*
*************************************************************
*Description: 白灯控制函数
*Arguments  : status:0 关闭   1 打开
              duty:占空比  (1-0xFF)/0xFF
              fade:1-淡入淡出，0-立即响应
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void white_led_control(u8 status,u8 duty, tbool fade)
{
    static u8 duty_save=0;
    u8 duty_tmp = 0;

//    P_FUNCTION
//    printf("status=  %d, duty= %d,fade= %d \n",status,duty,fade);

    //put_buf((void*)&light_attr,sizeof(LIGHT_ATTRIBUTE));

    if(status)
    {
        duty_tmp = (duty*light_attr.brightness)/255;
        rgb_led_control(0,0,0,0,LED_FADE_OFF);

        if (duty_tmp)
        {
//            puts("#1\n");
            WHITE_LED_OPEN();
        }

        if(!set_brightness_flag)
        {
//            puts("#2\n");
            light_attr.duty[WHITE_LED] = duty;
        }

    }
    else
    {
        WHITE_LED_CLOSE();
        if(!set_brightness_flag)
        {
            light_attr.duty[WHITE_LED] = 0;
        }
    }

//putchar('w');
//put_u8hex(duty_tmp);
//putchar('\n');

    if (duty_save!=duty_tmp)
    {
///        printf("ww_duty= %d\n",duty_tmp);
        duty_save = duty_tmp;
        led_w_pwm_ctrl(duty_save,fade);
    }
}

/*
*************************************************************
*Description: RGB灯控制函数
*Arguments  : status:0 关闭   1 打开
              r_duty:红灯占空比  (1-0xFF)/0xFF
              g_duty:绿灯占空比  (1-0xFF)/0xFF
              b_duty:蓝灯占空比  (1-0xFF)/0xFF
              fade:1-淡入淡出，0-立即响应
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void rgb_led_control(u8 status,u8 r_duty,u8 g_duty,u8 b_duty, tbool fade)
{
    static u8 r_duty_save=0;
    static u8 g_duty_save=0;
    static u8 b_duty_save=0;
    u8 r_duty_tmp = 0;
    u8 g_duty_tmp = 0;
    u8 b_duty_tmp = 0;

//    P_FUNCTION
    /* printf("status= %d, r_duty= %d,g_duty= %d,b_duty= %d,fade= %d \n",status,r_duty,g_duty,b_duty,fade); */

    if(status)
    {
        r_duty_tmp = (r_duty*light_attr.brightness)/255;
        g_duty_tmp = (g_duty*light_attr.brightness)/255;
        b_duty_tmp = (b_duty*light_attr.brightness)/255;

        white_led_control(0,0,LED_FADE_OFF);
        if (r_duty_tmp)
            R_LED_OPEN();
        if (g_duty_tmp)
            G_LED_OPEN();
        if (b_duty_tmp)
            B_LED_OPEN();

        if(!set_brightness_flag)
            light_attr.duty[RED_LED] = r_duty;
        if(!set_brightness_flag)
            light_attr.duty[GREEN_LED] = g_duty;
        if(!set_brightness_flag)
            light_attr.duty[BLUE_LED] = b_duty;
    }
    else
    {
        R_LED_CLOSE();
        G_LED_CLOSE();
        B_LED_CLOSE();
        if(!set_brightness_flag)
        {
            light_attr.duty[RED_LED] = 0;
            light_attr.duty[GREEN_LED] = 0;
            light_attr.duty[BLUE_LED] = 0;
        }
    }

//putchar('r');
//put_u8hex(r_duty_tmp);
//putchar('g');
//put_u8hex(g_duty_tmp);
//putchar('b');
//put_u8hex(b_duty_tmp);
//putchar('\n');

    if (r_duty_save!=r_duty_tmp)
    {
//        printf("rr_duty= %d\n",r_duty_tmp);
        r_duty_save = r_duty_tmp;
        led_r_pwm_ctrl(r_duty_save,fade);       //控制红灯的PWM
    }
    if (g_duty_save!=g_duty_tmp)
    {
//        printf("gg_duty= %d\n",g_duty_tmp);
        g_duty_save = g_duty_tmp;
        led_g_pwm_ctrl(g_duty_save,fade);   //控制绿灯的PWM
    }
    if (b_duty_save!=b_duty_tmp)
    {
//        printf("bb_duty= %d\n",b_duty_tmp);
        b_duty_save = b_duty_tmp;
        led_b_pwm_ctrl(b_duty_save,fade);   //控制蓝灯的PWM
    }
}

/*
*************************************************************
*Description: 彩灯闪烁处理函数
*Arguments  : mode:闪烁模式
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void led_flicker_deal(u8 mode)
{
    colorful_flag = 0;
    switch(mode)
    {    /*******彩色********/
    case COLORFUL_SLOW_FLICKER:
        colorful_flag = 1;
        flicker_effect = LED_FLASH_500MS;
        break;
    case COLORFUL_GRADUAL_FLICKER:
        colorful_flag = 1;
        flicker_effect = LED_FLASH_300MS;
        break;
    case COLORFUL_FAST_FLICKER:
        colorful_flag = 1;
        flicker_effect = LED_FLASH_100MS;
        break;
    /*******红色********/    case RED_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 0;
        break;
    case RED_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 0;
        break;
    case RED_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 0;
        break;
    /*******橙色********/    case ORANGE_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 1;
        break;
    case ORANGE_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 1;
        break;
    case ORANGE_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 1;
        break;
    /*******黄色********/    case YELLOW_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 2;
        break;
    case YELLOW_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 2;
        break;
    case YELLOW_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 2;
        break;
    /*******绿色********/
    case GREEN_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 3;
        break;
    case GREEN_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 3;
        break;
    case GREEN_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 3;
        break;
    /*******青色********/
    case CYAN_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 4;
        break;
    case CYAN_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 4;
        break;
    case CYAN_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 4;
        break;
    /*******蓝色********/
    case BLUE_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 5;
        break;
    case BLUE_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 5;
        break;
    case BLUE_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 5;
        break;
    /*******紫色********/
    case PURPLE_SLOW_FLICKER:
        flicker_effect = LED_FLASH_500MS;
        led_colour_cnt = 6;
        break;
    case PURPLE_GRADUAL_FLICKER:
        flicker_effect = LED_FLASH_300MS;
        led_colour_cnt = 6;
        break;
    case PURPLE_FAST_FLICKER:
        flicker_effect = LED_FLASH_100MS;
        led_colour_cnt = 6;
        break;
    default:
        break;
    }
}

/*
*************************************************************
*Description: 彩灯闪烁扫描函数
*Arguments  : flag--on/off
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
static u8 led_flicker_flag = 0;//闪烁标记

void check_ficker_scan(volatile u8 flag)
{
    static u8 colorful_cnt=0;

    if(flag)
    {
        if(led_flicker_flag == 1)
        {
            led_flicker_flag = 0;
            if(!colorful_flag)
            {
                rgb_led_control(1,colorful[led_colour_cnt][0],colorful[led_colour_cnt][1],colorful[led_colour_cnt][2],LED_FADE_OFF);
            }
            else
            {
                rgb_led_control(1,colorful[colorful_cnt][0],colorful[colorful_cnt][1],colorful[colorful_cnt][2],LED_FADE_OFF);
                colorful_cnt++;
                if(colorful_cnt >= 7)
                {
                    colorful_cnt = 0;
                }
            }
        }
    }
    else
    {
        led_flicker_flag = 1;
        if(!colorful_flag)
        {
            rgb_led_control(0,0,0,0,LED_FADE_OFF);
        }

    }
}

void led_flicker_deal_scan(void)
{

    if(light_attr.light_mode != LED_FLICKER)
        return;

    if(flicker_effect == LED_FLASH_100MS)
    {
        check_ficker_scan(time_100ms_flag);
    }
    else if(flicker_effect == LED_FLASH_300MS)
    {
        check_ficker_scan(time_300ms_flag);
    }
    else if(flicker_effect == LED_FLASH_500MS)
    {
        check_ficker_scan(time_500ms_flag);
    }
}

/*
*************************************************************
*Description: 音乐闪烁处理函数
*Arguments  : mode:闪烁模式
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void music_flicker_deal(u8 mode)
{
    colorful_flag = 0;
    switch(mode)
    {
    /*******彩色********/
    case COLORFUL_MUSIC_FLICKER:
        colorful_flag = 1;
        break;
    /*******红色********/
    case RED_MUSIC_FLICKER:
        music_colour_cnt = 0;
        break;
    /*******橙色********/
    case ORANGE_MUSIC_FLICKER:
        music_colour_cnt = 1;
        break;
    /*******黄色********/
    case YELLOW_MUSIC_FLICKER:
        music_colour_cnt = 2;
        break;
    /*******绿色********/
    case GREEN_MUSIC_FLICKER:
        music_colour_cnt = 3;
        break;
    /*******青色********/
    case CYAN_MUSIC_FLICKER:
        music_colour_cnt = 4;
        break;
    /*******蓝色********/
    case BLUE_MUSIC_FLICKER:
        music_colour_cnt = 5;
        break;
    /*******紫色********/
    case PURPLE_MUSIC_FLICKER:
        music_colour_cnt = 6;
        break;
    default:
        break;
    }
}

/*
*************************************************************
*Description: 音乐闪烁扫描处理函数
*Arguments  : NULL
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
static u8 music_flicker_flag = 0;

void music_flicker_deal_scan(void)
{
    u8 coefficient;
    u8 r_duty;
    u8 g_duty;
    u8 b_duty;
    u16 dac_energy = 0;
    static u8 colorful_cnt1=0;

    if(light_attr.light_mode != MUSIC_FLICKER)
        return;

    if(time_50ms_flag)
    {
        if(music_flicker_flag)
        {
            music_flicker_flag = 0;
            dac_energy = get_dac_energy_value();//get_voice_energy();
            if(dac_energy < 10)
            {
                if(!colorful_flag)
                {
                    rgb_led_control(1,colorful[music_colour_cnt][0],colorful[music_colour_cnt][1],colorful[music_colour_cnt][2],LED_FADE_OFF);
                }
                else
                {
                    rgb_led_control(1,colorful[colorful_cnt1][0],colorful[colorful_cnt1][1],colorful[colorful_cnt1][2],LED_FADE_OFF);
                }
            }
            else
            {
                coefficient = dac_energy / 255;
                coefficient = coefficient * ENERGY_GAIN;
//                if(coefficient < ENERGY_GAIN+1)
//                    coefficient = 0;
//                else if(coefficient > 255)
//                    coefficient = 255;
                if(coefficient > 255)
                    coefficient = 255;
                if(!colorful_flag)
                {
                    r_duty = colorful[music_colour_cnt][0]*coefficient/255;
                    g_duty = colorful[music_colour_cnt][1]*coefficient/255;
                    b_duty = colorful[music_colour_cnt][2]*coefficient/255;
                }
                else
                {
                    r_duty = colorful[colorful_cnt1][0]*coefficient/255;
                    g_duty = colorful[colorful_cnt1][1]*coefficient/255;
                    b_duty = colorful[colorful_cnt1][2]*coefficient/255;
                    colorful_cnt1++;
                    if(colorful_cnt1 >= 7)
                        colorful_cnt1 = 0;
                }

//                printf("%d %d %d\n",r_duty,g_duty,b_duty);
                rgb_led_control(1,r_duty,g_duty,b_duty,LED_FADE_OFF);
            }
        }
    }
    else
    {
        music_flicker_flag = 1;
    }
}

light_flash_t *get_time_target_p(u8 scene_type,u16 target_time)
{
    u16 cnt;
    u16 count = light_scene_control[scene_type].tab_cnt;

    light_flash_t *p = light_scene_control[scene_type].flash_struct;

    for(cnt = 0; cnt < count; cnt++)
    {
        if(p->time_order > target_time)
        {
            return p;
        }
        p++;
    }
    return NULL;
}

void led_scene_ram_ctl(u8 malloc_flag)
{
    if(malloc_flag)
    {
        if(light_scene_control == NULL)
        {
            rcsp_db_puts("-----scene malloc\n");
            light_scene_control = malloc((sizeof(light_disp_t) + 6) *  LGT_SCENE_UNIT_MAX);

            if(light_scene_control == NULL)
            {
                rcsp_err_puts("scene malloc fail!!!\n");
                return;
            }

            light_scene_target = (void*)&light_scene_control[LGT_SCENE_UNIT_MAX];
            light_scene_cur = (void*)&light_scene_target[LGT_SCENE_UNIT_MAX];
            light_scene_step = (void*)&light_scene_cur[LGT_SCENE_UNIT_MAX];

        }
    }
    else
    {
        if(light_scene_control != NULL)
        {
            free(light_scene_control);
            light_scene_control = NULL;
            rcsp_db_puts("-----scene free\n");
        }
    }
}


#define CHANGE_COLOR_STEP_VAL   20
u32 led_scene_1s_reflash(void)
{
    light_flash_t *flash_p;
    u8 reflash_flag,cnt;
    u16 scene_freq;

    static u8 older_save[LGT_SCENE_UNIT_MAX];

    if((light_scene_s_cnt == 0) || (light_scene_s_cnt >= light_scene_cycle))
    {
        memset(older_save,0xFF,LGT_SCENE_UNIT_MAX);
        light_scene_s_cnt = 0;
    }

//    printf("\n\nscene_1s, %d\n",light_scene_s_cnt);

    for(cnt = 0; cnt < LGT_SCENE_UNIT_MAX; cnt++)
    {
        reflash_flag = 0;

        flash_p = get_time_target_p(cnt,light_scene_s_cnt);

        if(flash_p == NULL)
        {
            rcsp_err_printf("reset=  %d\n",cnt);
            flash_p = light_scene_control[cnt].flash_struct;
            reflash_flag = 1;
        }
        else
        {
            if(older_save[cnt] != flash_p->time_order)
            {
                //cur flash_info
                reflash_flag = 1;
            }
        }

        if(reflash_flag)
        {
            older_save[cnt] = flash_p->time_order;

            if(cnt == LGT_SCENE_FREQ)
            {
                if(flash_p->time_value > 0)
                {
                    scene_freq = flash_p->start_value/flash_p->time_value;
                }
                else
                {
                    scene_freq = 0;
                }

                if(scene_freq > 0)
                {
                    if(scene_freq > 4)
                    {
                        light_scene_step[cnt] = 1;///50ms
                    }
                    else if(scene_freq == 4)
                    {
                        light_scene_step[cnt] = 2;///100ms
                    }

                    else if(scene_freq == 3)
                    {
                        light_scene_step[cnt] = 3;///150ms
                    }
                    else if(scene_freq == 2)
                    {
                        light_scene_step[cnt] = 5;///250ms
                    }
                    else
                    {
                        light_scene_step[cnt] = 10;///500ms
                    }
                }
                else
                {
                    light_scene_step[cnt] = 0;
                }

//                printf("freq: f= %d, t= %d\n",scene_freq,light_scene_step[cnt]);
            }
            else
            {
                light_scene_cur[cnt] = flash_p->start_value;
                light_scene_target[cnt] = flash_p->end_value;

                if(light_scene_cur[cnt] > light_scene_target[cnt])
                {
                    light_scene_step[cnt] =  light_scene_cur[cnt] - light_scene_target[cnt];
                }
                else if (light_scene_cur[cnt] < light_scene_target[cnt])
                {
                    light_scene_step[cnt] =  light_scene_target[cnt] - light_scene_cur[cnt];
                }
                else
                {
                    light_scene_step[cnt] = 0;
                }

                if(light_scene_step[cnt] > 0)
                {
                    light_scene_step[cnt] = light_scene_step[cnt]/(CHANGE_COLOR_STEP_VAL*flash_p->time_value) + 1;
                }

//                printf("cnt= %d, cur= %d, tag= %d, sep= %d,\n",cnt,light_scene_cur[cnt],light_scene_target[cnt],light_scene_step[cnt]);
            }

        }

    }

//    puts("\n");
//    printf("red:   cur= %d, tag= %d, sep= %d,\n",light_scene_cur[LGT_SCENE_RED],light_scene_target[LGT_SCENE_RED],light_scene_step[LGT_SCENE_RED]);
//    printf("green: cur= %d, tag= %d, sep= %d,\n",light_scene_cur[LGT_SCENE_GREEN],light_scene_target[LGT_SCENE_GREEN],light_scene_step[LGT_SCENE_GREEN]);
//    printf("blue:  cur= %d, tag= %d, sep= %d,\n",light_scene_cur[LGT_SCENE_BLUE],light_scene_target[LGT_SCENE_BLUE],light_scene_step[LGT_SCENE_BLUE]);
//    printf("FREQ:  fre= %d, t_count= %d,\n",scene_freq,light_scene_step[LGT_SCENE_FREQ]);
//    puts("\n");


    return 0;

}


void led_scene_deal_scan(void)
{
    u8 cnt;
    static u8 count_50ms = 0;
    static u8 scene_on_off_flag = 0;


    light_scene_10ms_cnt++;
    if(light_scene_10ms_cnt >= 100)
    {
        light_scene_s_cnt++;
        light_scene_10ms_cnt = 0;
    }

    if(light_scene_10ms_cnt == 0)
    {
        led_scene_1s_reflash();
        if(light_scene_step[LGT_SCENE_FREQ] == 3)
        {
            count_50ms = 0;
        }
    }

    if((light_scene_10ms_cnt%5) == 0)
    {
        count_50ms++;
        for(cnt = 0; cnt < LGT_SCENE_UNIT_MAX; cnt++)
        {
            if(light_scene_step[cnt] == 0)
            {
                if(cnt == LGT_SCENE_FREQ)
                {
                    scene_on_off_flag = 1;
                }
                continue;
            }

            if(cnt == LGT_SCENE_FREQ)
            {
                if(count_50ms >= light_scene_step[cnt])
                {
                    scene_on_off_flag = !scene_on_off_flag;
                    count_50ms = 0;
                }
            }
            else
            {
                if(light_scene_cur[cnt] < light_scene_target[cnt])
                {
                    if((light_scene_target[cnt] - light_scene_cur[cnt]) > light_scene_step[cnt])
                    {
                        light_scene_cur[cnt] += light_scene_step[cnt];
                    }
                    else
                    {
                        light_scene_cur[cnt] = light_scene_target[cnt];
                    }
                }
                else if(light_scene_cur[cnt] > light_scene_target[cnt])
                {
                    if((light_scene_cur[cnt] - light_scene_target[cnt]) > light_scene_step[cnt])
                    {
                        light_scene_cur[cnt] -= light_scene_step[cnt];
                    }
                    else
                    {
                        light_scene_cur[cnt] = light_scene_target[cnt];
                    }
                }
                else
                {
                    ;
                }
            }
        }

        if(scene_on_off_flag)
        {
            rgb_led_control(1,light_scene_cur[LGT_SCENE_RED],light_scene_cur[LGT_SCENE_GREEN],light_scene_cur[LGT_SCENE_BLUE],LED_FADE_OFF);
        }
        else
        {
            rgb_led_control(0,0,0,0,LED_FADE_OFF);
        }
    }

}

void chose_scene_deal(light_disp_t *scene_p,LIGHT_ATTRIBUTE *attr)
{
    u16 cnt;

    P_FUNCTION

    attr->light_mode = 0xff;
    attr->effect.enable = 0;
    white_led_control(0,0,LED_FADE_OFF);

//    put_buf((void*)scene_p,sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);

    memcpy(light_scene_control,scene_p,sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);

//    for(cnt = 0; cnt < LGT_SCENE_UNIT_MAX; cnt++)
//    {
//        printf("id--%d\n",cnt);
//        put_buf((void*)&scene_p[cnt],sizeof(light_disp_t));
//    }


    light_scene_cycle = 0xffff;

    for(cnt = 0; cnt < LGT_SCENE_UNIT_MAX; cnt++)
    {
        if((scene_p[cnt].tab_cycle > 0) && (scene_p[cnt].tab_cycle < light_scene_cycle))
        {
            light_scene_cycle =  scene_p[cnt].tab_cycle;
        }

    }

//    printf("cycle= %d\n",light_scene_cycle);

    light_scene_s_cnt = 0;
    light_scene_10ms_cnt = 0xffff;
    attr->light_mode = LED_SCENE_CTL;

}

/*
*************************************************************
*Description: 灯模式处理函数
*Arguments  : *attr:灯属性
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void chose_mode_deal(LIGHT_ATTRIBUTE *attr)
{
    printf("chose_mode_deal\n");
    switch(attr->light_mode)
    {
    case OP_WHITE_LED:
        printf("11\n");
        rgb_led_control(0,0,0,0,LED_FADE_OFF);
        white_led_control(1,0xFF,LED_FADE_ON);
        attr->effect.enable = 0;
        break;

    case MUSIC_FLICKER:
        printf("22\n");
        white_led_control(0,0,LED_FADE_OFF);
        music_flicker_deal(attr->effect.mode);
        attr->effect.enable = 1;
        break;

    case LED_FLICKER:
        printf("33\n");
        white_led_control(0,0,LED_FADE_OFF);
        led_flicker_deal(attr->effect.mode);
        attr->effect.enable = 1;
        break;

    case LED_COLOUR:
        printf("44\n");
        white_led_control(0,0,LED_FADE_OFF);
        rgb_led_control(1,attr->duty[RED_LED],attr->duty[GREEN_LED],attr->duty[BLUE_LED],LED_FADE_OFF);
        attr->effect.enable = 0;
        break;

    case LED_SCENE_CTL:
        printf("77-scene\n");
        break;

    default:
        printf("55\n");
        attr->effect.enable = 0;
        break;
    }
}

/*
*************************************************************
*Description: 灯设置亮度函数
*Arguments  : value:灯亮度值
              fade:1-淡入淡出，0-立即响应
*Returns    : 0:成功    其他:失败
*Note       :
*By         :
*************************************************************
*/
void set_brightness(u8 value, tbool fade)
{
    printf("brightness:%d",value);
    if(value >= 255)
        light_attr.brightness = 255;
    else
        light_attr.brightness = value;

    set_brightness_flag = 1;

    if(light_attr.light_mode == OP_WHITE_LED)
    {
        white_led_control(1,light_attr.duty[WHITE_LED],fade);
    }
    else
    {
        rgb_led_control(1,light_attr.duty[RED_LED],light_attr.duty[GREEN_LED],light_attr.duty[BLUE_LED],fade);
    }
    set_brightness_flag = 0;
}



/*
*************************************************************
*Description: 灯闪烁处理函数,在定时器中断里完成
*Arguments  : NULL
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void led_light_effect(void)
{
    if (led_is_fading())
    {
        led_fade();
        return;
    }

    if(light_attr.light_mode == LED_SCENE_CTL)
    {
        led_scene_deal_scan();
    }

    if(light_attr.effect.enable)
    {
        led_flicker_deal_scan();
        music_flicker_deal_scan();
    }

}


/*
*************************************************************
*Description: 灯闪烁处理函数
*Arguments  :

cmd--                            LIGHT_CMD_E
mode--                           LIGHT_MODE_E
brightness--                     0~100
w_duty,r_duty,g_duty,b_duty--    0~255
en_effect--                      0 or 1
effect_md--                      LED_FLICKER_MODE

*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void light_control_deal_with(u16 cmd,LIGHT_MODE_E mode,u8 brightness,u8 w_duty,u8 r_duty,u8 g_duty,u8 b_duty,
                    u8 en_effect,u8 effect_md)
{
#if 1
    LIGHT_ATTRIBUTE attr;
    attr.light_mode = mode;
    attr.brightness = brightness;
    attr.duty[0] = w_duty;
    attr.duty[1] = r_duty;
    attr.duty[2] = g_duty;
    attr.duty[3] = b_duty;
    attr.effect.enable = en_effect;
    attr.effect.mode = effect_md;
    bt_control_data_deal(0, cmd,(u8 *)&attr,sizeof(LIGHT_ATTRIBUTE));
#endif
}


/*
*************************************************************
*Description: 灯初始化函数
*Arguments  : NULL
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void bt_led_init(void)
{
#if RCSP_LIGHT_HW

	P_FUNCTION
    if (led_init_flag > 0)
    {
        return;
    }

    led_fade_init();
    white_led_control(0,0,LED_FADE_OFF);
    rgb_led_control(0,0,0,0,LED_FADE_OFF);

    //light_attr结构体初始化
    memset((u8 *)&light_attr,0,sizeof(LIGHT_ATTRIBUTE));
    light_attr.light_mode = OP_WHITE_LED;
    light_attr.brightness = 0xFF;
    light_attr.duty[0] = 0xFF;
    light_attr.duty[1] = 0xFF;
    light_attr.duty[2] = 0xFF;
    light_attr.duty[3] = 0xFF;
    light_attr.effect.enable = 0;
    light_attr.effect.mode = 0;

//    light_control_deal_with(LIGHT_CMD_CHOSE_MODE,OP_WHITE_LED,0xFF,0,0,0,0,0,0);
//    light_control_deal_with(LIGHT_CMD_CHOSE_MODE,MUSIC_FLICKER,0xFF,0,0,0,0,0,CYAN_MUSIC_FLICKER);
//    light_control_deal_with(LIGHT_CMD_CHOSE_MODE,LED_FLICKER,0xFF,0,0,0,0,0,COLORFUL_SLOW_FLICKER);

///    light_control_deal_with(LIGHT_CMD_CHOSE_MODE,LED_COLOUR,0xFF,255,colorful[6][0],colorful[6][1],colorful[6][2],0,0);

//    white_led_control(1,0xFF,LED_FADE_ON);

    led_init_flag = 1;
    light_on_off = 0;
#endif
}


void light_timer_10ms(void)
{
#if RCSP_LIGHT_HW

	static u16 cnt_10ms = 0;

    if (0 == led_init_flag)
    {
        return;
    }

    cnt_10ms++;

    if((cnt_10ms % 5) == 0)
    {
        time_50ms_flag = 1;
    }

    if((cnt_10ms % 10) == 0)
    {
        time_100ms_flag = 1;
    }

    if((cnt_10ms % 30) == 0)
    {
        time_300ms_flag = 1;
    }

    if((cnt_10ms % 50) == 0)
    {
        time_500ms_flag = 1;
    }

    if(cnt_10ms == 150)
    {
        cnt_10ms = 0;
    }

    led_light_effect();

    time_50ms_flag = 0;
    time_100ms_flag = 0;
    time_300ms_flag = 0;
    time_500ms_flag = 0;

#endif	
	
}

/*
*************************************************************
*Description: 灯网络数据处理函数
*Arguments  : NULL
*Returns    : NULL
*Note       :
*By         :
*************************************************************
*/
void bt_control_data_deal(u16 devnum, u16 cmd,u8 *buf,u32 len)
{

#if 1
    u8 brightness_temp = 0;
    LIGHT_ATTRIBUTE *light_parm_p = (void*)buf;

    P_FUNCTION
    /* printf("cmd= %d\n",cmd); */

//    if(len < 16)
//        put_buf(buf,len);

    switch(cmd)
    {
    case LIGHT_CMD_ON:
        if(light_attr.light_mode == OP_WHITE_LED)
        {
            WHITE_LED_OPEN();
            white_led_control(1,light_attr.duty[WHITE_LED],LED_FADE_ON);
        }
        else
        {
            R_LED_OPEN();
            G_LED_OPEN();
            B_LED_OPEN();
            rgb_led_control(1,light_attr.duty[RED_LED],light_attr.duty[GREEN_LED],light_attr.duty[BLUE_LED],LED_FADE_ON);
        }
        light_on_off = 1;
        break;

    case LIGHT_CMD_OFF:
        light_attr.effect.enable = 0;
        light_attr.light_mode = LED_CLOSE_STATE;
        rgb_led_control(0,0,0,0,LED_FADE_OFF);
        white_led_control(0,0,LED_FADE_OFF);
        WHITE_LED_CLOSE();
        R_LED_CLOSE();
        G_LED_CLOSE();
        B_LED_CLOSE();
        light_on_off = 0;
        break;

    case LIGHT_CMD_BRIGHT:

//        if(light_parm_p->brightness > 100)
//        {
//            light_parm_p->brightness = 100;
//        }

        if(light_parm_p->brightness < 95)
        {
            light_parm_p->brightness = light_parm_p->brightness>>1;
        }

        set_brightness((255*light_parm_p->brightness)/100,LED_FADE_ON);  //亮度值转换,手机APP端发过来的亮度值是0-100级
        light_on_off = 1;
        break;

    case LIGHT_CMD_CHOSE_MODE:
        light_parm_p->brightness = light_attr.brightness;
        memcpy((u8 *)&light_attr,light_parm_p,sizeof(LIGHT_ATTRIBUTE));
        chose_mode_deal(&light_attr);
        light_on_off = 1;
        break;

    case LIGHT_CMD_CHOSE_SCENE:
        chose_scene_deal((void*)buf,&light_attr);
        light_on_off = 1;
        break;

    default:
        break;
    }

#endif

}

void test_fw_led_mode(void)
{
    static u8 t_count = 0;
    static u8 t_send_cnt = 0;

#if 0

    if (0 == led_init_flag)
    {
        return;
    }

    t_count++;

    if(t_count >= 30)
    {
        t_send_cnt++;
        t_count = 0;
    }
    else
    {
        return;
    }

    rcsp_db_puts("\n\n");

    P_FUNCTION

    printf("---t_send_cnt= %d\n",t_send_cnt);

    if(t_send_cnt == 1)
    {
        puts("--------------white--------------\n");
//         white_led_control(1,100,LED_FADE_ON);
        light_control_deal_with(LIGHT_CMD_CHOSE_MODE,OP_WHITE_LED,0xFF,0,0,0,0,0,0);
    }

    if(t_send_cnt == 2)
    {
        puts("--------------red--------------\n");
//        rgb_led_control(1,100,0,0,LED_FADE_ON);
        light_control_deal_with(LIGHT_CMD_CHOSE_MODE,MUSIC_FLICKER,0xFF,0,0,0,0,0,CYAN_MUSIC_FLICKER);
    }

    if(t_send_cnt == 3)
    {
        puts("--------------green--------------\n");
//        rgb_led_control(1,0,100,0,LED_FADE_ON);
        light_control_deal_with(LIGHT_CMD_CHOSE_MODE,LED_FLICKER,0xFF,0,0,0,0,0,COLORFUL_SLOW_FLICKER);
    }

    if(t_send_cnt >= 4)
    {
        puts("--------------blue--------------\n");
//        rgb_led_control(1,0,0,100,LED_FADE_ON);
        light_control_deal_with(LIGHT_CMD_CHOSE_MODE,LED_COLOUR,0xFF,255,colorful[6][0],colorful[6][1],colorful[6][2],0,0);
//        white_led_control(0,0,LED_FADE_OFF);
        t_send_cnt = 0;
    }

#endif

}

u8 get_effect_mode(u8 flash_md)
{
    u8 freq_mode;
    u8 flash_sn;

    freq_mode = flash_md >> 6;
    flash_sn = flash_md % MUSIC_FLICKER_MAX;

    if(freq_mode > 0)
    {
//#if 0
//        if(freq_mode == 1)
//        {
//            ///缓闪
//            freq_mode = 2;
//        }
//        else if(freq_mode == 2)
//        {
//            ///慢闪
//            freq_mode = 1;
//        }
//        else
//        {
//            ///快闪
//            ;
//        }
//#endif // 0
        flash_sn = (flash_sn * 3) + (freq_mode - 1); ///color flash
    }
    else
    {
        flash_sn |= 0x80; ///music flash
    }
    return flash_sn;
}

///set mode
///0--corlor,1--bright,2-scence
void light_control_set(void *set_param,u32 set_mode)
{

#if RCSP_LIGHT_HW

	LIGHT_ATTRIBUTE attr;
    light_info_t *light_set_p;
    u8 cmd = 0xFF;
    u8 tmp8;

    P_FUNCTION

    memset(&attr,0x00,sizeof(LIGHT_ATTRIBUTE));

    if(set_mode == 2)
    {
        led_scene_ram_ctl(1);//malloc ram

        if(light_scene_control == NULL)
        {
            return;
        }

        cmd = LIGHT_CMD_CHOSE_SCENE;
        bt_control_data_deal(0, cmd,(u8*)set_param,sizeof(light_disp_t)*LGT_SCENE_UNIT_MAX);
        rcsp_db_puts("new_light_attr00:");
        rcsp_put_buf((void*)&light_attr,sizeof(LIGHT_ATTRIBUTE));
        return;
    }

	puts("\nlight_set_param:");
	put_buf(set_param,sizeof(light_info_t));

    light_set_p = (light_info_t*)set_param;
    attr.light_mode = OP_WHITE_LED;
    attr.brightness = light_set_p->color_value[LGT_COLOR_BRIGHT];
    attr.duty[0] = light_set_p->color_value[LGT_COLOR_WHITE];//w_duty;
    attr.duty[1] = light_set_p->color_value[LGT_COLOR_RED];//r_duty;
    attr.duty[2] = light_set_p->color_value[LGT_COLOR_GREEN];//g_duty;
    attr.duty[3] = light_set_p->color_value[LGT_COLOR_BLUE];//b_duty;
    attr.effect.enable = 0;

    led_scene_ram_ctl(0);//free ram,after set light_mode

    if(light_set_p->button == 0)
    {
        cmd = LIGHT_CMD_OFF;
    }
    else
    {
        if(set_mode == 1)
        {
            cmd = LIGHT_CMD_BRIGHT;
        }
        else
        {
            switch(light_set_p->mode)
            {
            case LGT_MD_CLOSE:
                break;

            case LGT_MD_WHITE:
                if(light_set_p->color_value[LGT_COLOR_WHITE] > 0)
                {
                    cmd = LIGHT_CMD_CHOSE_MODE;
                    attr.light_mode = OP_WHITE_LED;
                }
                else
                {
                    cmd = LIGHT_CMD_OFF;
                }
                break;

            case LGT_MD_COLOR:
                cmd = LIGHT_CMD_CHOSE_MODE;
                attr.light_mode = LED_COLOUR;
                break;

            case LGT_MD_COLOR_FLASH:
                if((light_set_p->m_value[0] & 0x0f) == 0)
                {
                    cmd = LIGHT_CMD_OFF;
                }
                else
                {
                    cmd = LIGHT_CMD_CHOSE_MODE;
                    attr.effect.enable = 1;

                    tmp8 = get_effect_mode(light_set_p->m_value[0] - 1);
                    if(tmp8 & 0x80)
                    {
                        attr.light_mode = MUSIC_FLICKER;
                    }
                    else
                    {
                        attr.light_mode = LED_FLICKER;
                    }
                    attr.effect.mode = tmp8 & 0x7f;
                }
                break;

            case LGT_MD_SCENE:
                break;

            default:
                break;

            }
        }
    }

end:
    if(cmd != 0xff)
    {
        bt_control_data_deal(0, cmd,(u8*)&attr,sizeof(LIGHT_ATTRIBUTE));
    }
    else
    {
        rcsp_db_puts("unknow cmd---\n");
    }

    rcsp_db_puts("new_light_attr:");
    rcsp_put_buf((void*)&light_attr,sizeof(LIGHT_ATTRIBUTE));

#endif
}

#endif


