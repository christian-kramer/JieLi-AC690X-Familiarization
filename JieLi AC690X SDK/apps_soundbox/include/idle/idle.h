#ifndef IDLE_TASK_H
#define IDLE_TASK_H

#include "includes.h"

#define IDLE_DEBUG
#ifdef IDLE_DEBUG
    #define idle_puts     puts
    #define idle_printf   printf
#else
    #define idle_puts(...)
    #define idle_printf(...)
#endif

#define IDLE_TASK_NAME   "IdleTask"

extern const struct task_info idle_task_info;
extern const struct task_info light_task_info;

#endif

