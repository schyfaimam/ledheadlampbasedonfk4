/*===========================================================================*/
/* Module      = icu_feret.h                                                 */
/* Version     = V1.01                                                       */
/*               generated by DeFiXE2 0.8.4.5                                */
/*===========================================================================*/
/*                                  COPYRIGHT                                */
/*===========================================================================*/
/* Copyright (c) 2010 by Renesas Electronics Europe GmbH,                    */
/*               a company of the Renesas Electronics Corporation            */
/*===========================================================================*/
/* Purpose:      Definition of ICU Macros                                    */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* Warranty Disclaimer                                                       */
/*                                                                           */
/* Because the Product(s) is licensed free of charge, there is no warranty   */
/* of any kind whatsoever and expressly disclaimed and excluded by NEC,      */
/* either expressed or implied, including but not limited to those for       */
/* non-infringement of intellectual property, merchantability and/or         */
/* fitness for the particular purpose.                                       */
/* NEC shall not have any obligation to maintain, service or provide bug     */
/* fixes for the supplied Product(s) and/or the Application.                 */
/*                                                                           */
/* Each User is solely responsible for determining the appropriateness of    */
/* using the Product(s) and assumes all risks associated with its exercise   */
/* of rights under this Agreement, including, but not limited to the risks   */
/* and costs of program errors, compliance with applicable laws, damage to   */
/* or loss of data, programs or equipment, and unavailability or             */
/* interruption of operations.                                               */
/*                                                                           */
/* Limitation of Liability                                                   */
/*                                                                           */
/* In no event shall NEC be liable to the User for any incidental,           */
/* consequential, indirect, or punitive damage (including but not limited    */
/* to lost profits) regardless of whether such liability is based on breach  */
/* of contract, tort, strict liability, breach of warranties, failure of     */
/* essential purpose or otherwise and even if advised of the possibility of  */
/* such damages. NEC shall not be liable for any services or products        */
/* provided by third party vendors, developers or consultants identified or  */
/* referred to the User by NEC in connection with the Product(s) and/or the  */
/* Application.                                                              */
/*                                                                           */
/*===========================================================================*/
/* Environment:                                                              */
/*              Device:         all V850E2 core devices                      */
/*              IDE:            GHS Multi for V800                           */
/*===========================================================================*/

#ifndef __ICU_FERET_H__
 #define __ICU_FERET_H__

 #pragma asm
 #if (__V800_registermode==22)
   .set  regframe, 16
 #endif
 #if (__V800_registermode==26)
   .set  regframe, 18
 #endif
 #if (__V800_registermode==32)
   .set  regframe, 21
 #endif

   .set  framesize, regframe*4
 #pragma endasm

 asm void FETRAP_ENTRY( void)
 {
    -- isr prolog 
    -- 22 reg mode: save 15 registers + BSEL + FEPSW + FEPC
    -- 26 reg mode: save 17 registers + BSEL + FEPSW + FEPC
    -- 32 reg mode: save 20 registers + BSEL + FEPSW + FEPC
    prepare   {ep-lp}, regframe, sp
    sst.w     r1,  (framesize-4)[ep]

    stsr      BSEL, r1
    sst.w     r1, (framesize-8)[ep]     -- save actual bank #
    ldsr      r0, BSEL                  -- set bank/group # to 0

    stsr      FEPSW, r1
    sst.w     r1,  (framesize-12)[ep]

    stsr      FEPC, r1
    sst.w     r1,  (framesize-16)[ep]

    sst.w     r2,  (framesize-20)[ep]
    sst.w     gp,  (framesize-24)[ep]
    sst.w     r5,  (framesize-28)[ep]
    sst.w     r6,  (framesize-32)[ep]
    sst.w     r7,  (framesize-36)[ep]
    sst.w     r8,  (framesize-40)[ep]
    sst.w     r9,  (framesize-44)[ep]
    sst.w     r10, (framesize-48)[ep]
    sst.w     r11, (framesize-52)[ep]
    sst.w     r12, (framesize-56)[ep]
    sst.w     r13, (framesize-60)[ep]
    sst.w     r14, (framesize-64)[ep]
 #if (__V800_registermode==26)
    sst.w     r15, (framesize-68)[ep]
    sst.w     r16, (framesize-72)[ep]
 #endif
 #if (__V800_registermode==32)
    sst.w     r15, (framesize-68)[ep]
    sst.w     r16, (framesize-72)[ep]
    sst.w     r17, (framesize-76)[ep]
    sst.w     r18, (framesize-80)[ep]
    sst.w     r19, (framesize-84)[ep]
 #endif
 }

 asm void FETRAP_LEAVE( void)
 {
    mov       sp, ep       
 #if (__V800_registermode==32)
    sld.w     (framesize-84)[ep], r19
    sld.w     (framesize-80)[ep], r18
    sld.w     (framesize-76)[ep], r17
    sld.w     (framesize-72)[ep], r16
    sld.w     (framesize-68)[ep], r15
 #endif
 #if (__V800_registermode==26)
    sld.w     (framesize-72)[ep], r16
    sld.w     (framesize-68)[ep], r15
 #endif
    sld.w     (framesize-64)[ep], r14
    sld.w     (framesize-60)[ep], r13
    sld.w     (framesize-56)[ep], r12
    sld.w     (framesize-52)[ep], r11
    sld.w     (framesize-48)[ep], r10
    sld.w     (framesize-44)[ep], r9
    sld.w     (framesize-40)[ep], r8
    sld.w     (framesize-36)[ep], r7
    sld.w     (framesize-32)[ep], r6
    sld.w     (framesize-28)[ep], r5
    sld.w     (framesize-24)[ep], gp
    sld.w     (framesize-20)[ep], r2

    di
    sld.w     (framesize-16)[ep], r1
    ldsr      r1, FEPC

    sld.w     (framesize-12)[ep], r1
    ldsr      r1, FEPSW

    sld.w     (framesize-8)[ep], r1
    ldsr      r1, BSEL

    sld.w     (framesize-4)[ep], r1
    dispose   regframe, {ep-lp}
    feret  
 }
 #define PRAGMA(x) _Pragma(#x)

 #define FETRAP_EXCEPTION( name, isr) \
   PRAGMA( ghs noprologue)            \
   void name( void) {                 \
     FETRAP_ENTRY();                  \
     isr();                           \
     FETRAP_LEAVE();                  \
   } 


 #endif  // __ICU_FERET_H__
