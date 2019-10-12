


#ifndef __PMU__ 
#define __PMU__


#define pmu_csen()      (PD_CON |= BIT(0))
#define pmu_csdis()     (PD_CON &= ~BIT(0))
#define pmu_stran()     (PD_CON |= BIT(5))
#define pmu_swait()     (PD_CON & BIT(1))


#define pmu_go()        (PD_CON |= BIT(4))
#define pmu_wto()       (PD_CON & BIT(7))


#define RD_PMU_CON0         (0x00)
#define WR_PMU_CON0         (RD_PMU_CON0 | 0x80)
// {TOPND, CLRPND, PDRUN, TOEN, DISEN[1:0], PDMD, PDEN}


#define RD_PMU_CON1         (0x01)
#define WR_PMU_CON1         (RD_PMU_CON1 | 0x80)
// {PORFLAG, CLRFLAG, CKDIV[1:0], PUSLOW, CKSEL[2:0]}


#define RD_SOFT_FLAG        (0x02)
#define WR_SOFT_FLAG        (RD_SOFT_FLAG | 0x80)
// {SFLAG[7:0]}


#define RD_PWR_SCON         (0x03)
#define WR_PWR_SCON         (RD_PWR_SCON | 0x80)
// {reserved[7:2], WLDO12_EN, WFM30_EN}


#define RD_STB10_SET        (0x04)
#define WR_STB10_SET        (RD_STB10_SET | 0x80)
// {STB1_SET[3:0], STB0_SET[3:0]}
#define RD_STB32_SET        (0x05)
#define WR_STB32_SET        (RD_STB32_SET | 0x80)
// {STB3_SET[3:0], STB2_SET[3:0]}
#define RD_STB54_SET        (0x06)
#define WR_STB54_SET        (RD_STB54_SET | 0x80)
// {STB5_SET[3:0], STB4_SET[3:0]}
#define RD_STB6_SET         (0x07)
#define WR_STB6_SET         (RD_STB6_SET | 0x80)
// {MSTM[3:0], STB6_SET[3:0]}


#define RD_LVC3_SET         (0x08)
#define WR_LVC3_SET         (RD_LVC3_SET | 0x80)
// {LVC[31:24]}
#define RD_LVC2_SET         (0x09)
#define WR_LVC2_SET         (RD_LVC2_SET | 0x80)
// {LVC[23:16]}
#define RD_LVC1_SET         (0x0a)
#define WR_LVC1_SET         (RD_LVC1_SET | 0x80)
// {LVC[15:8]}
#define RD_LVC0_SET         (0x0b)
#define WR_LVC0_SET         (RD_LVC0_SET | 0x80)
// {LVC[7:0]}


#define RD_RSC3_SET         (0x0c)
#define WR_RSC3_SET         (RD_RSC3_SET | 0x80)
// {RSC[31:24]}
#define RD_RSC2_SET         (0x0d)
#define WR_RSC2_SET         (RD_RSC2_SET | 0x80)
// {RSC[23:16]}
#define RD_RSC1_SET         (0x0e)
#define WR_RSC1_SET         (RD_RSC1_SET | 0x80)
// {RSC[15:8]}
#define RD_RSC0_SET         (0x0f)
#define WR_RSC0_SET         (RD_RSC0_SET | 0x80)
// {RSC[15:8]}


#define RD_PRC1_SET         (0x10)
#define WR_PRC1_SET         (RD_PRC1_SET | 0x80)
// {PRC[15:8]}
#define RD_PRC0_SET         (0x11)
#define WR_PRC0_SET         (RD_PRC0_SET | 0x80)
// {PRC[7:0]}


#define RD_STC1_READ        (0x12)
// {STC[15:8]}
#define RD_STC0_READ        (0x13)
// {STC[7:0]}


#define RD_LVC3_READ        (0x14)
// {LVC[31:24]}
#define RD_LVC2_READ        (0x15)
// {LVC[23:16]}
#define RD_LVC1_READ        (0x16)
// {LVC[15:8]}
#define RD_LVC0_READ        (0x17)
// {LVC[7:0]}


#define RD_MD_CON           (0x1c)
#define WR_MD_CON           (RD_MD_CON | 0x80)
// {reserved[7:1], LVDIS}


#define RD_IVS_READ         (0x1d)
// {PWVLD, M12VLD, M33VLD, SWVLD, ALLVLD, ANALVD, CLKVLD, NONVO}
#define WR_IVS_SET1         (0x1e | 0x80)
// {PWVLD, M12VLD, M33VLD, SWVLD, ALLVLD, ANALVD, CLKVLD, NONVO}
#define WR_IVS_SET0         (0x1f | 0x80)
// {PWVLD, M12VLD, M33VLD, SWVLD, ALLVLD, ANALVD, CLKVLD, NONVO}



#endif



