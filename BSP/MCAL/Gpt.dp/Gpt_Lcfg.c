/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* File name    = Gpt_Lcfg.c                                                  */
/* Version      = 3.1.2                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2012 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Link-time parameters                                    */
/* AUTOMATICALLY GENERATED FILE - DO NOT EDIT                                 */
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
**                       Generation Tool Version                              **
*******************************************************************************/
/*
 * TOOL VERSION:    3.1.6
 */

/*******************************************************************************
**                         Input File                                         **
*******************************************************************************/
/*
 * INPUT FILE:    FK4_4010_GPT_V308_140113_HEADLAMP.arxml
 * GENERATED ON:  15 Jan 2014 - 13:14:57
 */

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "Gpt.h"
#include "Gpt_Cbk.h"
#include "Gpt_LTTypes.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/*
 * AUTOSAR specification version information
 */
#define GPT_LCFG_C_AR_MAJOR_VERSION  2
#define GPT_LCFG_C_AR_MINOR_VERSION  2
#define GPT_LCFG_C_AR_PATCH_VERSION  0
/*
 * File version information
 */
#define GPT_LCFG_C_SW_MAJOR_VERSION    3
#define GPT_LCFG_C_SW_MINOR_VERSION    1

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

#if (GPT_LTTYPES_AR_MAJOR_VERSION != GPT_LCFG_C_AR_MAJOR_VERSION)
  #error "Gpt_Lcfg.c : Mismatch in Specification Major Version"
#endif

#if (GPT_LTTYPES_AR_MINOR_VERSION != GPT_LCFG_C_AR_MINOR_VERSION)
  #error "Gpt_Lcfg.c : Mismatch in Specification Minor Version"
#endif

#if (GPT_LTTYPES_AR_PATCH_VERSION != GPT_LCFG_C_AR_PATCH_VERSION)
  #error "Gpt_Lcfg.c : Mismatch in Specification Patch Version"
#endif

#if (GPT_LTTYPES_SW_MAJOR_VERSION != GPT_LCFG_C_SW_MAJOR_VERSION)
  #error "Gpt_Lcfg.c : Mismatch in Major Version"
#endif

#if (GPT_LTTYPES_SW_MINOR_VERSION != GPT_LCFG_C_SW_MINOR_VERSION)
  #error "Gpt_Lcfg.c : Mismatch in Minor Version"
#endif

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

#define GPT_START_SEC_CONST_UNSPECIFIED
#include "MemMap.h"

/* RAM Initialization of GPT Channel Configuration */
CONST(Tdd_Gpt_ChannelFuncType, GPT_CONST) Gpt_GstChannelFunc[] =
{
  /* Channel: 0 */
  {
    /* pGptNotifPtr */
    TAUJ0I0_Notification
  }
};



#define GPT_STOP_SEC_CONST_UNSPECIFIED
#include "MemMap.h"

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
