
//***************************************************************************//
//                                                                           //
//                    vDSP extend instructions define                        //
//                                                             by:zequan_liu //
//***************************************************************************//
 
//------------------------------------------------------//
// new register group 
//------------------------------------------------------//
 
  #define GRP_IE0   0x7 // 0x7    // ie0
  #define REG_IE0   0x7 // 0x7
                       
  #define GRP_CFG   0x7 // 0x7    // icfg
  #define REG_CFG   0x2 // 0x4
                       
  #define GRP_ACH   0x4 // 0x7    // macch
  #define REG_ACH   0x4 // 0x0
                       
  #define GRP_ACL   0x4 // 0x7    // maccl
  #define REG_ACL   0x5 // 0x1
                       
  #define GRP_TMR   0x5 // 0x6    // cycles
  #define REG_TMR   0x4 // 0x6

  #define GRP_PRD   0x5 // 0x6    // period
  #define REG_PRD   0x5 // 0x7

//------------------------------------------------------//
// asm extend signal
//------------------------------------------------------//

  #define G0      0x0
  #define G1      0x1
  #define G2      0x2
  #define G3      0x3
  #define G4      0x4
  #define G5      0x5
  #define G6      0x6
  #define G7      0x7
                  
  #define DR0     0x0
  #define DR1     0x1
  #define DR2     0x2
  #define DR3     0x3
  #define DR4     0x4
  #define DR5     0x5
  #define DR6     0x6
  #define DR7     0x7

//------------------------------------------------------//
// special register remapping
//------------------------------------------------------//
                  
  #define SSP6     SP
  #define USP6     USP      // TODO SYSCFG
                  
  #define IE0      EMUDAT
  #define ICFG     SYSCFG   // TODO RETX

  #define hi(adr)  (adr>>16)
  #define lo(adr)  (adr&0xffff)

//------------------------------------------------------//
// cpp extend instrucion
//------------------------------------------------------//

  #define idle                        asm volatile ("IDLE")
  #define IDLE                        asm volatile ("IDLE")
  #define cpp_IFLUSH(Px)              asm volatile ("IFLUSH  [%0];" :: "p"(Px));
  #define cpp_FLUSH(Px)               asm volatile ("FLUSH   [%0];" :: "p"(Px));
  #define cpp_FLUSHINV(Px)            asm volatile ("FLUSHINV[%0];" :: "p"(Px));
  #define cpp_PREFLUSH(Px)            asm volatile ("PREFLUSH[%0];" :: "p"(Px));
                                    
  #define cpp_TESTCLR(Px)             STR_B(Px,0)
  #define cpp_TESTSET(Px)             asm volatile ("TESTSET(%0); if !cc jump -2;" ::"p"(Px));
                                      
  #define LOCK                        asm volatile (".byte2 0x0026")
  #define UNLOCK                      asm volatile (".byte2 0x0027")
                                    
  #define MUL64U(Rm,Rn)               asm volatile (".word 0xc512; DIVQ(%0,%1);" :: "r"(Rm), "r"(Rn));
  #define MAC64U(Rm,Rn)               asm volatile (".word 0xc512; %0=(%0+%1)<<1;" :: "r"(Rm), "r"(Rn));
  #define MAS64U(Rm,Rn)               asm volatile (".word 0xc512; %0=(%0+%1)<<2;" :: "r"(Rm), "r"(Rn));
  #define MUL64S(Rm,Rn)               asm volatile (".word 0xc512; DIVS(%0,%1);" :: "r"(Rm), "r"(Rn));
  #define MAC64S(Rm,Rn)               asm volatile (".word 0xc512; %0*=%1;" :: "r"(Rm), "r"(Rn));
  #define MAS64S(Rm,Rn)               asm volatile (".word 0xc512; %0=~%1;" :: "r"(Rm), "r"(Rn));
  #define ACC64_SSHF_U6(Rd,u6)        asm volatile (".word 0xc512; %0+=%1;" : "=r"(Rd) : "i"(u6));
  #define ACC64_USHF_U6(Rd,u6)        asm volatile (".word 0xc512; %0 =%1;" : "=r"(Rd) : "i"(u6));
  #define ACC64_SSHF_R6(Rd,Rm)        asm volatile (".word 0xc512; %0>>>=%1;" : "=r"(Rd) : "r"(Rm));
  #define ACC64_USHF_R6(Rd,Rm)        asm volatile (".word 0xc512; %0 >>=%1;" : "=r"(Rd) : "r"(Rm));
  #define MUL32U(Rd,Rm,Rn)            asm volatile (".word 0xc512; %0=%1&%2;" : "=r"(Rd) : "r"(Rm), "r"(Rn));
                                    
//#define ACC64_RES(Rdh,Rdl)          asm volatile ("%0=USP; %1=SEQSTAT;" : "=r"(Rdh), "=r"(Rdl));
//#define ACC64_SET(Rdh,Rdl)          asm volatile ("USP=%0; SEQSTAT=%1;" : :"r"(Rdh), "r"(Rdl));

//------------------------------------------------------//
// asm extend instrucion
//------------------------------------------------------//

  #define ext_IDLE                   .BYTE2 0x0020
  #define ext_LOCK                   .BYTE2 0x0026
  #define ext_UNLOCK                 .BYTE2 0x0027
                                   
  #define ext_MUL64U(Rm,Rn)          .BYTE2 0xc512, 0x4200|(Rn<<3)|(Rm<<0)
  #define ext_MAC64U(Rm,Rn)          .BYTE2 0xc512, 0x4100|(Rn<<3)|(Rm<<0)
  #define ext_MAS64U(Rm,Rn)          .BYTE2 0xc512, 0x4140|(Rn<<3)|(Rm<<0)
  #define ext_MUL64S(Rm,Rn)          .BYTE2 0xc512, 0x4240|(Rn<<3)|(Rm<<0)
  #define ext_MAC64S(Rm,Rn)          .BYTE2 0xc512, 0x40c0|(Rn<<3)|(Rm<<0)
  #define ext_MAS64S(Rm,Rn)          .BYTE2 0xc512, 0x43c0|(Rn<<3)|(Rm<<0)
  #define ext_ACC64_SSHF_U6(Rd,u6)   .BYTE2 0xc512, 0x6400|(u6<<3)|(Rd<<0)
  #define ext_ACC64_USHF_U6(Rd,u6)   .BYTE2 0xc512, 0x6000|(u6<<3)|(Rd<<0)
  #define ext_ACC64_SSHF_R6(Rd,Rm)   .BYTE2 0xc512, 0x4000|(Rm<<3)|(Rd<<0)
  #define ext_ACC64_USHF_R6(Rd,Rm)   .BYTE2 0xc512, 0x4040|(Rm<<3)|(Rd<<0)
  #define ext_MUL(Rd,Rm,Rn)          .BYTE2 0xc512, 0x5400|(Rd<<9)|(Rm<<3)|(Rn<<0)
  #define ext_MOV(Gd,Rd,Gs,Rs)       .BYTE2 0x3000|(Gd<<9)|(Gs<<6)|(Rd<<3)|(Rs<<0)
  #define ext_PUSH(Gs,Rs)            .BYTE2 0x0140|(Gs<<3)|(Rs<<0)
  #define ext_POP(Gd,Rd)             .BYTE2 0x0100|(Gd<<3)|(Rd<<0)

//***************************************************************************//
//                                file end                                   //
//***************************************************************************//

