#ifndef JIFFIES_H
#define JIFFIES_H


extern volatile unsigned long jiffies;


#define time_after_bt(a,b)						 ((long)(b) - (long)(a) < 0)
#define time_before_bt(a,b)			     time_after_bt(b, a)


#define msecs_to_jiffies_2(msec)       ((msec)/2)

#define msecs_to_jiffies_10(msec)       ((msec)/10)
//#define msecs_to_jiffies_100(msec)       ((msec)/100)
#define jiffies_to_msecs(j)       ((j)*10)

#define jiffies_to_msecs_10(j)     (((j)*625UL)/10000)


#endif

