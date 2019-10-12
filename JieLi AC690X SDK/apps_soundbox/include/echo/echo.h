#ifndef _ENCHO_H_
#define _ENCHO_H_

#include "includes.h"

#define ECHO_TASK_NAME  "ECHO_TASK"


// #define ECHO_DEBUG
#ifdef ECHO_DEBUG
    #define echo_puts     puts
    #define echo_printf   printf
#else
    #define echo_puts(...)
    #define echo_printf(...)
#endif

///#define BPF_ECHO_MP3       BPF_CONNECT_MP3
#define ECHO_CHANNEL       DAC_DIGITAL_AVOL

extern const struct task_info echo_task_info;


#endif
