/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Gpt_Ram.c                                                   */
/* Version      = 3.1.4                                                       */
/* Date         = 06-Jun-2012                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2012 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Global variable definitions of GPT Driver               */
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

/*****************************************************************************
**                      Revision Control History                            **
*****************************************************************************/
/*
 * V3.0.0:  23-Oct-2009    : Initial Version
 * V3.0.1:  25-Feb-2010    : As per SCR 194, precompile option is added for the
 *                           pointer variable "Gpt_GpTAUUnitConfig".
 * V3.0.2:  23-Jun-2010    : As per SCR 281, precompile option is updated for
 *                           the pointer variable "Gpt_GpTAUUnitConfig".
 * V3.0.3:  08-Jul-2010    : As per SCR 299, precompile option
 *                           "GPT_DEV_ERROR_DETECT" is added for
 *                           Initializing Gpt_GblDriverStatus.
 * V3.1.0:  27-Jul-2011    : Modified software version to 3.1.0
 * V3.1.1:  04-Oct-2011    : Added comments for the violation of MISRA rule 19.1
 * V3.1.3:  16-Feb-2012    : Merged the fixes done for Fx4L Gpt driver
 * V3.1.4:  06-Jun-2012    : As per SCR 029, Environment section is updated to
 *                           remove compiler version.
 */
/*****************************************************************************/

/******************************************************************************
**                      Include Section                                      **
******************************************************************************/
#include "Gpt.h"
#include "Gpt_Ram.h"

/******************************************************************************
**                      Version Information                                  **
******************************************************************************/

/* AUTOSAR Specification version information */
#define GPT_RAM_C_AR_MAJOR_VERSION    GPT_AR_MAJOR_VERSION_VALUE
#define GPT_RAM_C_AR_MINOR_VERSION    GPT_AR_MINOR_VERSION_VALUE
#define GPT_RAM_C_AR_PATCH_VERSION    GPT_AR_PATCH_VERSION_VALUE

/* File version information */
#define GPT_RAM_C_SW_MAJOR_VERSION    3
#define GPT_RAM_C_SW_MINOR_VERSION    1

/******************************************************************************
**                      Version Check                                        **
******************************************************************************/
#if (GPT_RAM_AR_MAJOR_VERSION != GPT_RAM_C_AR_MAJOR_VERSION)
  #error "Gpt_Ram.c : Mismatch in Specification Major Version"
#endif

#if (GPT_RAM_AR_MINOR_VERSION != GPT_RAM_C_AR_MINOR_VERSION)
  #error "Gpt_Ram.c : Mismatch in Specification Minor Version"
#endif

#if (GPT_RAM_AR_PATCH_VERSION != GPT_RAM_C_AR_PATCH_VERSION)
  #error "Gpt_Ram.c : Mismatch in Specification Patch Version"
#endif

#if (GPT_SW_MAJOR_VERSION != GPT_RAM_C_SW_MAJOR_VERSION)
  #error "Gpt_Ram.c : Mismatch in Major Version"
#endif

#if (GPT_SW_MINOR_VERSION != GPT_RAM_C_SW_MINOR_VERSION)
  #error "Gpt_Ram.c : Mismatch in Minor Version"
#endif

/******************************************************************************
**                         Global Data                                       **
******************************************************************************/
#define GPT_START_SEC_VAR_NOINIT_UNSPECIFIED
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

#if((GPT_TAUA_UNIT_USED == STD_ON)||(GPT_TAUJ_UNIT_USED == STD_ON) \
    || (GPT_TAUB_UNIT_USED == STD_ON)||(GPT_TAUC_UNIT_USED == STD_ON))
/* Global pointer variable for TAU Unit configuration */
P2CONST(Tdd_Gpt_TAUUnitConfigType, GPT_CONST, GPT_CONFIG_CONST)
       Gpt_GpTAUUnitConfig;
#endif

/* Global pointer variable for channel configuration */
P2CONST(Tdd_Gpt_ChannelConfigType, GPT_CONST, GPT_CONFIG_CONST)
  Gpt_GpChannelConfig;

/* Global pointer variable for channel to timer mapping */
P2CONST(uint8, GPT_CONST, GPT_CONFIG_CONST) Gpt_GpChannelTimerMap;

/* Global pointer variable for channel data */
P2VAR(Tdd_Gpt_ChannelRamData, GPT_NOINIT_DATA, GPT_CONFIG_DATA)
  Gpt_GpChannelRamData;

#define GPT_STOP_SEC_VAR_NOINIT_UNSPECIFIED
#include "MemMap.h"/* PRQA S 5087 */

#define GPT_START_SEC_VAR_NOINIT_8BIT
#include "MemMap.h"/* PRQA S 5087 */

/* Holds the status of GPT Driver Component */
VAR(uint8, GPT_NOINIT_DATA) Gpt_GucGptDriverMode;

#define GPT_STOP_SEC_VAR_NOINIT_8BIT
#include "MemMap.h"/* PRQA S 5087 */


#define GPT_START_SEC_VAR_1BIT
#include "MemMap.h"/* PRQA S 5087 */

#if (GPT_DEV_ERROR_DETECT == STD_ON)
/* Holds the status of Initialization */
VAR(boolean, GPT_INIT_DATA) Gpt_GblDriverStatus = GPT_UNINITIALIZED;
#endif

#define GPT_STOP_SEC_VAR_1BIT
#include "MemMap.h"/* PRQA S 5087 */

/******************************************************************************
**                      Function Definitions                                 **
******************************************************************************/

/******************************************************************************
**                          End of File                                      **
******************************************************************************/
