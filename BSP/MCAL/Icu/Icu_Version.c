/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Icu_Version.c                                               */
/* Version      = 3.0.0a                                                      */
/* Date         = 18-Oct-2011                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2011 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains version check of modules included by ICU Driver         */
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
 * V3.0.0:  26-Aug-2009  : Initial version
 * V3.0.0a: 18-Oct-2011  : Copyright is updated.
 */
/******************************************************************************/


/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "Icu.h"
#include "Icu_Version.h"
#if(ICU_CRITICAL_SECTION_PROTECTION == STD_ON)
#include "SchM.h"
#endif
#if (ICU_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif
#if (ICU_REPORT_WAKEUP_SOURCE == STD_ON)
#include "EcuM.h"
#endif
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/*
 * AUTOSAR specification version information
 */
#define ICU_VERSION_C_AR_MAJOR_VERSION    ICU_AR_MAJOR_VERSION_VALUE
#define ICU_VERSION_C_AR_MINOR_VERSION    ICU_AR_MINOR_VERSION_VALUE
#define ICU_VERSION_C_AR_PATCH_VERSION    ICU_AR_PATCH_VERSION_VALUE

/* File version information */
#define ICU_VERSION_C_SW_MAJOR_VERSION    3
#define ICU_VERSION_C_SW_MINOR_VERSION    0

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

/* Icu Version Check */
#if (ICU_VERSION_AR_MAJOR_VERSION != ICU_VERSION_C_AR_MAJOR_VERSION)
  #error "Icu_Version.c : Mismatch in Specification Major Version"
#endif

#if (ICU_VERSION_AR_MINOR_VERSION != ICU_VERSION_C_AR_MINOR_VERSION)
  #error "Icu_Version.c : Mismatch in Specification Minor Version"
#endif

#if (ICU_VERSION_AR_PATCH_VERSION != ICU_VERSION_C_AR_PATCH_VERSION)
  #error "Icu_Version.c : Mismatch in Specification Patch Version"
#endif

#if (ICU_VERSION_SW_MAJOR_VERSION != ICU_VERSION_C_SW_MAJOR_VERSION)
  #error "Icu_Version.c : Mismatch in Major Version"
#endif

#if (ICU_VERSION_SW_MINOR_VERSION != ICU_VERSION_C_SW_MINOR_VERSION)
  #error "Icu_Version.c : Mismatch in Minor Version"
#endif

/* Det Module Version Check */
#if (ICU_DEV_ERROR_DETECT == STD_ON)
#if (DET_AR_MAJOR_VERSION != ICU_DET_AR_MAJOR_VERSION)
  #error "Det.h : Mismatch in Specification Major Version"
#endif

#if (DET_AR_MINOR_VERSION != ICU_DET_AR_MINOR_VERSION)
  #error "Det.h : Mismatch in Specification Minor Version"
#endif
#endif /* #if (ICU_DEV_ERROR_DETECT == STD_ON) */

#if(ICU_CRITICAL_SECTION_PROTECTION == STD_ON)
/* SchM Module Version Check */
#if (SCHM_AR_MAJOR_VERSION != ICU_SCHM_AR_MAJOR_VERSION)
  #error "SchM.h : Mismatch in Specification Major Version"
#endif

#if (SCHM_AR_MINOR_VERSION != ICU_SCHM_AR_MINOR_VERSION)
  #error "SchM.h : Mismatch in Specification Minor Version"
#endif
#endif

/* EcuM Module Version Check */
#if (ICU_REPORT_WAKEUP_SOURCE == STD_ON)
#if (ECUM_AR_MAJOR_VERSION != ICU_ECUM_AR_MAJOR_VERSION)
  #error "EcuM.h : Mismatch in Specification Major Version"
#endif

#if (ECUM_AR_MINOR_VERSION != ICU_ECUM_AR_MINOR_VERSION)
  #error "EcuM.h : Mismatch in Specification Minor Version"
#endif
#endif /* #if (ICU_REPORT_WAKEUP_SOURCE == STD_ON) */


/*******************************************************************************
**                         Global Symbols                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
