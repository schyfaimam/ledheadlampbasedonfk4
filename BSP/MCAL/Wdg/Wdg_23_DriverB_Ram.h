/*============================================================================*/
/* Project      = AUTOSAR NEC MCAL Components                                 */
/* Module       = Wdg_23_DriverB_Ram.h                                        */
/* Version      = 3.0.1                                                       */
/* Date         = 14-Jul-2009                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009 by NEC Electronics Corporation                          */
/*============================================================================*/
/* Purpose:                                                                   */
/* Global RAM macro/variable definitions for Watchdog Driver                  */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* NEC Electronics Corporation the following shall apply!                     */
/*                                                                            */
/* Warranty Disclaimer                                                        */
/*                                                                            */
/* There is no warranty of any kind whatsoever granted by NEC. Any warranty   */
/* is expressly disclaimed and excluded by NEC, either expressed or implied,  */
/* including but not limited to those for non-infringement of intellectual    */
/* property, merchantability and/or fitness for the particular purpose.       */
/*                                                                            */
/* NEC shall not have any obligation to maintain, service or provide bug      */
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
/* In no event shall NEC be liable to the User for any incidental,            */
/* consequential, indirect, or punitive damage (including but not limited     */
/* to lost profits) regardless of whether such liability is based on breach   */
/* of contract, tort, strict liability, breach of warranties, failure of      */
/* essential purpose or otherwise and even if advised of the possibility of   */
/* such damages. NEC shall not be liable for any services or products         */
/* provided by third party vendors, developers or consultants identified or   */
/* referred to the User by NEC in connection with the Product(s) and/or the   */
/* Application.                                                               */
/*                                                                            */
/*============================================================================*/
/* Environment:                                                               */
/*              Devices:        Xx4                                           */
/*              Compiler:       GHS V5.1.6c                                   */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V3.0.0:  12-Jun-2009  : Initial version.
 * V3.0.1:  14-Jul-2009  : As per SCR 015, compiler version is changed from 
 *                         V5.0.5 to V5.1.6c in the header of the file.
 */
/******************************************************************************/

#ifndef WDG_23_DRIVERB_RAM_H
#define WDG_23_DRIVERB_RAM_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "Wdg_23_DriverB.h"
#include "Wdg_23_DriverB_PBTypes.h"
#include "WdgIf_Types.h"
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/* Autosar Specification Version */

#define WDG_23_DRIVERB_RAM_AR_MAJOR_VERSION \
                                           WDG_23_DRIVERB_AR_MAJOR_VERSION_VALUE
#define WDG_23_DRIVERB_RAM_AR_MINOR_VERSION \
                                           WDG_23_DRIVERB_AR_MINOR_VERSION_VALUE
#define WDG_23_DRIVERB_RAM_AR_PATCH_VERSION \
                                           WDG_23_DRIVERB_AR_PATCH_VERSION_VALUE

/* File version information */

#define WDG_23_DRIVERB_RAM_SW_MAJOR_VERSION 3
#define WDG_23_DRIVERB_RAM_SW_MINOR_VERSION 0


#if (WDG_23_DRIVERB_DEV_ERROR_DETECT == STD_ON)

#define WDG_23_DRIVERB_START_SEC_VAR_UNSPECIFIED 
#include "MemMap.h"

/* Global variable to store the driver state */
extern VAR(Wdg_23_DriverB_StatusType, WDG_23_DRIVERB_INIT_DATA) 
                                                 Wdg_GddDriverState;
#define WDG_23_DRIVERB_STOP_SEC_VAR_UNSPECIFIED 
#include "MemMap.h"

#endif

#define WDG_23_DRIVERB_START_SEC_VAR_NOINIT_UNSPECIFIED 
#include "MemMap.h"

/* Global variable to store the current watchdog state */
extern VAR(WdgIf_ModeType, WDG_23_DRIVERB_NOINIT_DATA) Wdg_GddCurrentMode;

/* Global variable to store pointer to Configuration */
extern P2CONST(Wdg_23_DriverB_ConfigType, AUTOMATIC, 
              WDG_23_DRIVERB_CONFIG_CONST)Wdg_23_DriverB_GpConfigPtr;

#define WDG_23_DRIVERB_STOP_SEC_VAR_NOINIT_UNSPECIFIED
#include "MemMap.h"

/*******************************************************************************
**                      Global Function Prototypes                            **
*******************************************************************************/

#endif /* WDG_23_DRIVERB_RAM_H */

/*******************************************************************************
**                          End Of File                                       **
*******************************************************************************/
