/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Pwm_Irq.c                                                   */
/* Version      = 3.1.4                                                       */
/* Date         = 06-Jun-2012                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2012 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* ISR functions for all Timers of the PWM Driver Component                   */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* Renesas Electronics Corporation the following shall apply!                 */
/*                                                                            */
/* Warranty Disclaimer                                                        */
/*                                                                            */
/* There is no warranty of any kind whatsoever granted by Renesas. Any        */
/* warranty is expressly disclaimed and excluded by Renesas, either expressed */
/* or implied, including but not limited to those for non-infringement of     */
/* intellectual property, merchantability and/or fitness for the particular   */
/* purpose.                                                                   */
/*                                                                            */
/* Renesas shall not have any obligation to maintain, service or provide bug  */
/* fixes for the supplied Product(s) and/or the Application.                  */
/*                                                                            */
/* Each User is solely responsible for determining the appropriateness of     */
/* using the Product(s) and assumes all risks associated with its exercise    */
/* of rights under this Agreement, including, but not limited to the risks    */
/* and costs of program errors, compliance with applicable laws, damage to    */
/* or loss of data, programs or equipment, and unavailability or              */
/* interruption of operations.                                                */
/*                                                                            */
/* Limitation of Liability                                                    */
/*                                                                            */
/* In no event shall Renesas be liable to the User for any incidental,        */
/* consequential, indirect, or punitive damage (including but not limited     */
/* to lost profits) regardless of whether such liability is based on breach   */
/* of contract, tort, strict liability, breach of warranties, failure of      */
/* essential purpose or otherwise and even if advised of the possibility of   */
/* such damages. Renesas shall not be liable for any services or products     */
/* provided by third party vendors, developers or consultants identified or   */
/* referred to the User by Renesas in connection with the Product(s) and/or   */
/* the Application.                                                           */
/*                                                                            */
/*============================================================================*/
/* Environment:                                                               */
/*              Devices:        Xx4                                           */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V3.0.0:  18-Aug-2009  : Initial version
 * V3.0.1:  28-Oct-2009  : As per the SCR 054 Pwm_HW_Callback function is called
 *                         in ISR for optimization.
 * V3.0.2:  02-Jul-2010  : As per SCR 290, following changes are made:
 *                         1. ISR for for TAUB1 and TAUC2-TAUC8 are added.
 *                         2. File is updated to support ISR Category support,
 *                            configurable by a pre-compile option.
 * V3.0.3:  25-Jul-2010  : As per SCR 305, ISR Category options are updated
 *                         from MODE to TYPE.
 * V3.0.4:  03-Jan-2011  : As per SCR 387, TAUB1_CH0's Interrupt routines
 *                         are corrected.
 *
 * V3.1.0:  26-Jul-2011  : Ported for the DK4 variant.
 * V3.1.1:  04-Oct-2011  : Added comments for the violation of MISRA rule 19.1.
 * V3.1.3:  05-Mar-2012  : Merged the fixes done for Fx4L PWM driver
 * V3.1.4:  06-Jun-2012  : As per SCR 034, Compiler version is removed from
 *                         Environment section.
 */
/******************************************************************************/


/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Pwm.h"
#include "Pwm_Irq.h"
#include "Pwm_Ram.h"
#include "Pwm_LLDriver.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR Specification version information */
#define PWM_IRQ_C_AR_MAJOR_VERSION    PWM_AR_MAJOR_VERSION_VALUE
#define PWM_IRQ_C_AR_MINOR_VERSION    PWM_AR_MINOR_VERSION_VALUE
#define PWM_IRQ_C_AR_PATCH_VERSION    PWM_AR_PATCH_VERSION_VALUE

/* File version information */
#define PWM_IRQ_C_SW_MAJOR_VERSION    3
#define PWM_IRQ_C_SW_MINOR_VERSION    1

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
#if (PWM_IRQ_AR_MAJOR_VERSION != PWM_IRQ_C_AR_MAJOR_VERSION)
  #error "Pwm_Irq.c : Mismatch in Specification Major Version"
#endif

#if (PWM_IRQ_AR_MINOR_VERSION != PWM_IRQ_C_AR_MINOR_VERSION)
  #error "Pwm_Irq.c : Mismatch in Specification Minor Version"
#endif

#if (PWM_IRQ_AR_PATCH_VERSION != PWM_IRQ_C_AR_PATCH_VERSION)
  #error "Pwm_Irq.c : Mismatch in Specification Patch Version"
#endif

#if (PWM_SW_MAJOR_VERSION != PWM_IRQ_C_SW_MAJOR_VERSION)
  #error "Pwm_Irq.c : Mismatch in Major Version"
#endif

#if (PWM_SW_MINOR_VERSION != PWM_IRQ_C_SW_MINOR_VERSION)
  #error "Pwm_Irq.c : Mismatch in Minor Version"
#endif

/******************************************************************************
**                         Global Data                                       **
******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*
 * The below ISRs will be checked only if PWM_NOTIFICATION_SUPPORTED is
 * STD_ON
 */
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
/*******************************************************************************
** Function Name        : TAUAn_CHm_ISR
**
** Service ID           : NA
**
** Description          : These are Interrupt routines for the timer TAUAn and
**                        m, where n represents the TAUA Units and m
**                        represents channels associated for each Unit.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant
**
** Input Parameters     : None
**
** InOut Parameters     : None
**
** Output Parameters    : None
**
** Return parameter     : None
**
** Preconditions        : None
**
** Remarks              : Global Variable(s):
**                        Pwm_GpChannelTimerMap, Pwm_GpChannelConfig,
**                        Pwm_GstChannelNotifFunc, Pwm_GpNotifStatus
**
**                        Function(s) invoked:
**                        None
*******************************************************************************/
#if (PWM_TAUA_UNIT_USED == STD_ON)/*PWM_TAUA_UNIT_USED == STD_ON*/

#if (PWM_TAUA0_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
  /* MISRA Rule         : 19.1                            */
  /* Message            : #include statements in a file   */
  /*                      should only be preceded by other*/
  /*                      preprocessor directives or      */
  /*                      comments.                       */
  /* Reason             : For code optimization.          */
  /* Verification       : However, part of the code is    */
  /*                      verified manually and it is not */
  /*                      having any impact.              */
#include "MemMap.h"

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA0_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA0_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA0_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA0_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA0_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA1_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA1_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA1_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA1_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA1_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA2_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA2_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA2_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA2_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA2_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA3_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA3_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA3_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA3_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA3_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA4_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA4_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA4_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA4_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA4_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA5_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA5_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA5_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA5_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA5_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA6_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA6_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA6_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA6_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA6_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA7_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA7_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA7_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA7_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA7_CH15_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH0_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH1_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH2_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH3_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH4_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH5_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH6_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH7_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH8_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH9_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH10_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH11_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH12_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH13_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH14_ISR_API == STD_ON) */

#if (PWM_TAUA8_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUA8_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUA8_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUA8_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUA8_CH15_ISR_API == STD_ON) */
#endif /*PWM_TAUA_UNIT_USED == STD_ON*/

#if (PWM_TAUB_UNIT_USED == STD_ON)/*PWM_TAUB_UNIT_USED == STD_ON*/
#if (PWM_TAUB0_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH0_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH1_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH2_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH3_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH4_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH5_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH6_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH7_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH8_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH9_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH10_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH11_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH12_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH13_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH14_ISR_API == STD_ON) */

#if (PWM_TAUB0_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB0_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB0_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB0_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB0_CH15_ISR_API == STD_ON) */


#if (PWM_TAUB1_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH0_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH1_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH2_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH3_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH4_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH5_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH6_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH7_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH8_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH9_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH10_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH11_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH12_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH13_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH14_ISR_API == STD_ON) */

#if (PWM_TAUB1_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB1_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB1_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB1_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB1_CH15_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH0_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH1_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH2_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH3_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH4_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH5_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH6_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH7_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH8_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH9_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH10_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH11_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH12_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH13_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH14_ISR_API == STD_ON) */

#if (PWM_TAUB2_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUB2_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUB2_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUB2_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUB2_CH15_ISR_API == STD_ON) */

#endif /*PWM_TAUB_UNIT_USED == STD_ON*/

#if (PWM_TAUC_UNIT_USED == STD_ON)/*PWM_TAUC_UNIT_USED == STD_ON*/

#if (PWM_TAUC2_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC2_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC2_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC2_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC2_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC2_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC3_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC3_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC3_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC3_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC3_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC4_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC4_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC4_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC4_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC4_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC5_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC5_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC5_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC5_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC5_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC6_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC6_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC6_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC6_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC6_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC7_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC7_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC7_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC7_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC7_CH15_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH0_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH1_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH2_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH3_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH4_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH4_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH4_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH4_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH4);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH4_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH5_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH5_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH5_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH5_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH5);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH5_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH6_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH6_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH6_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH6_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH6);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH6_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH7_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH7_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH7_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH7_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH7);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH7_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH8_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH8_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH8_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH8_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH8);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH8_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH9_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH9_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH9_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH9_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH9);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH9_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH10_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH10_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH10_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH10_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH10);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH10_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH11_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH11_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH11_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH11_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH11);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH11_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH12_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH12_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH12_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH12_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH12);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH12_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH13_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH13_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH13_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH13_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH13);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH13_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH14_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH14_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH14_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH14_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH14);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH14_ISR_API == STD_ON) */

#if (PWM_TAUC8_CH15_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH15_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUC8_CH15_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUC8_CH15_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUC8_CH15);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUC8_CH15_ISR_API == STD_ON) */

#endif/*PWM_TAUC_UNIT_USED == STD_ON*/

#if (PWM_TAUJ_UNIT_USED == STD_ON)/*PWM_TAUJ_UNIT_USED == STD_ON*/
#if (PWM_TAUJ0_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ0_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ0_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ0_CH0_ISR_API == STD_ON) */

#if (PWM_TAUJ0_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ0_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ0_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ0_CH1_ISR_API == STD_ON) */

#if (PWM_TAUJ0_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ0_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ0_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ0_CH2_ISR_API == STD_ON) */

#if (PWM_TAUJ0_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ0_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ0_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ0_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ0_CH3_ISR_API == STD_ON) */

#if (PWM_TAUJ1_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ1_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ1_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ1_CH0_ISR_API == STD_ON) */

#if (PWM_TAUJ1_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ1_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ1_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ1_CH1_ISR_API == STD_ON) */

#if (PWM_TAUJ1_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ1_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ1_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ1_CH2_ISR_API == STD_ON) */

#if (PWM_TAUJ1_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ1_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ1_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ1_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ1_CH3_ISR_API == STD_ON) */

#if (PWM_TAUJ2_CH0_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH0_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ2_CH0_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH0_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ2_CH0);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ2_CH0_ISR_API == STD_ON) */

#if (PWM_TAUJ2_CH1_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH1_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ2_CH1_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH1_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ2_CH1);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ2_CH1_ISR_API == STD_ON) */

#if (PWM_TAUJ2_CH2_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH2_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ2_CH2_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH2_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ2_CH2);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ2_CH2_ISR_API == STD_ON) */

#if (PWM_TAUJ2_CH3_ISR_API == STD_ON)
#define PWM_START_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */

/* MCAL_ISR_TYPE_TOOLCHAIN - Toolchain defines the interrupt mapping */
#if (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_TOOLCHAIN)
_INTERRUPT_ FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH3_ISR(void)

/* MCAL_ISR_TYPE_OS - OS declares the ISR and defines the interrupt mapping */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_OS)
ISR(TAUJ2_CH3_ISR)

/* MCAL_ISR_TYPE_NONE - user writed his own dispatcher for ISR */
#elif (PWM_INTERRUPT_TYPE == MCAL_ISR_TYPE_NONE)
FUNC(void, PWM_PUBLIC_CODE) TAUJ2_CH3_ISR(void)
#endif
{
  Pwm_HW_Callback(PWM_TAUJ2_CH3);
}

#define PWM_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"/* PRQA S 5087 */
#endif /* #if (PWM_TAUJ2_CH3_ISR_API == STD_ON) */
#endif/*PWM_TAUJ_UNIT_USED == STD_ON*/
#endif/* End of (PWM_NOTIFICATION_SUPPORTED == STD_ON) */


/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
