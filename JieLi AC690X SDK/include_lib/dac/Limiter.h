#ifndef Limiter_h__
#define Limiter_h__

typedef struct __Limiter Limiter;
//ReleaseTime and AttackTime is in ms
//TargetLvl is in mdB 0~-20
//SampleRate in Hz
//nChannel 1~2
void Limiter_Init(Limiter *limiter, int ReleaseTime, int AttackTime, int TargetLvl, int SampleRate, int nChannel);
void Limiter_SetVariable(Limiter *limiter, int ReleaseTime, int AttackTime, int TargerLvl, int SampleRate);
int Limiter_QueryBufSize(int x);
void Limiter_Run(Limiter *limiter, int *in, int *out, int npoint);
#endif // Limiter_new_h__
