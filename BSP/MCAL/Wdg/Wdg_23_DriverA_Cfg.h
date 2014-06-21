/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Wdg_23_DriverA_Cfg.h                                        */
/* Version      = 3.0.0                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2011 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains pre-compile time parameters.                            */
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
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V3.0.0:              : Initial version
 */
/******************************************************************************/
/*******************************************************************************
**                   Generation Tool Version                                  **
*******************************************************************************/
/*
 * TOOL VERSION:  3.0.8a 
 */

/*******************************************************************************
**                         Input File                                         **
*******************************************************************************/

/*
 * INPUT FILE:    FK4_4010_WDGA_V304_120319.arxml
 * GENERATED ON:  19 Mar 2012 - 13:36:07
 */

#ifndef WDG_23_DRIVERA_CFG_H
#define WDG_23_DRIVERA_CFG_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR specification version information */
#define WDG_23_DRIVERA_CFG_AR_MAJOR_VERSION 2
#define WDG_23_DRIVERA_CFG_AR_MINOR_VERSION 2
#define WDG_23_DRIVERA_CFG_AR_PATCH_VERSION 0


/* File version information */
#define WDG_23_DRIVERA_CFG_SW_MAJOR_VERSION 3
#define WDG_23_DRIVERA_CFG_SW_MINOR_VERSION 0

/*******************************************************************************
**                      Common Published Information                          **
*******************************************************************************/
#define WDG_23_DRIVERA_AR_MAJOR_VERSION_VALUE 2 
#define WDG_23_DRIVERA_AR_MINOR_VERSION_VALUE 2 
#define WDG_23_DRIVERA_AR_PATCH_VERSION_VALUE 0 

#define WDG_23_DRIVERA_SW_MAJOR_VERSION_VALUE 3 
#define WDG_23_DRIVERA_SW_MINOR_VERSION_VALUE 0 
#define WDG_23_DRIVERA_SW_PATCH_VERSION_VALUE 0 

#define WDG_23_DRIVERA_VENDOR_ID_VALUE (uint16)23 
#define WDG_23_DRIVERA_MODULE_ID_VALUE (uint16)102 


/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/* Pre-compile option for development error detect */
#define WDG_23_DRIVERA_DEV_ERROR_DETECT     STD_OFF

/* Pre-compile option for version info API */
#define WDG_23_DRIVERA_VERSION_INFO_API     STD_OFF

/* Compile switch to allow/forbid disabling Watchdog Driver during runtime */
#define WDG_23_DRIVERA_DISABLE_ALLOWED      STD_ON

/* Compile switch to allow/forbid VAC */
#define WDG_23_DRIVERA_VAC_ALLOWED          STD_OFF

/* Watchdog Driver Id */
#define WDG_23_DRIVERA_INDEX                (uint8)0


/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/* Instance ID of the WDG Component*/
#define WDG_23_DRIVERA_INSTANCE_ID_VALUE    (uint8)0

/* Minimum Watchdog Timer timeout in milliseconds */
#define WDG_23_DRIVERA_MIN_TIMEOUT          (float32)0.00213

/* Maximum Watchdog Timer timeout in milliseconds*/
#define WDG_23_DRIVERA_MAX_TIMEOUT          (float32)139.81013

/* Resolution of Watchdog timeout period in milliseconds*/
#define WDG_23_DRIVERA_RESOLUTION           (float32)0.0000041666

/* Watchdog trigger mode */
#define WDG_23_DRIVERA_TRIGGER_MODE         WDG_WINDOW

/* Configuration Set Handles */
#define WdgModeConfig0                      &Wdg_23_DriverA_GstConfiguration[0]

/* Address of Trigger Register when VAC is OFF */
#define WDG_23_DRIVERA_WDTAWDTE_ADDRESS     *((volatile uint8 *)0xFF806000ul)


/* Address of Mode Register */
#define WDG_23_DRIVERA_WDTAMD_ADDRESS       *((volatile uint8 *)0xFF80600Cul)



/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* WDG_23_DRIVERA_CFG_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
