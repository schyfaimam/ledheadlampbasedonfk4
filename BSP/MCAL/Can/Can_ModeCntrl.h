/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Can_ModeCntrl.h                                             */
/* Version      = 3.0.1a                                                      */
/* Date         = 18-Oct-2011                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2008-2011 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* C header file for Can_ModeCntrl.c                                          */
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
 * V3.0.0:  12.12.2008  : Initial Version
 * V3.0.1:  21.10.2009  : Updated compiler version as per
 *                        SCR ANMCANLINFR3_SCR_031.
 * V3.0.1a: 18-Oct-2011  : Copyright is updated.
 */
/******************************************************************************/

#ifndef CAN_MODECNTRL_H
#define CAN_MODECNTRL_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR specification version information */
#define CAN_MODECNTRL_AR_MAJOR_VERSION  2
#define CAN_MODECNTRL_AR_MINOR_VERSION  2
#define CAN_MODECNTRL_AR_PATCH_VERSION  2

/* File version information */
#define CAN_MODECNTRL_SW_MAJOR_VERSION  3
#define CAN_MODECNTRL_SW_MINOR_VERSION  0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/* Structure for ModeFuncPtr */
typedef struct STagTdd_Can_AFCan_ModeFuncPtr
{
  P2FUNC(Can_ReturnType,CAN_AFCAN_PRIVATE_CODE,pModeFuncPtr)
(P2CONST(Can_ControllerConfigType,AUTOMATIC,
                                          CAN_AFCAN_PRIVATE_CONST)LpController);
}Tdd_Can_AFCan_ModeFuncPtr;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define CAN_AFCAN_START_SEC_PRIVATE_CODE
#include "MemMap.h"

extern FUNC(Can_ReturnType, CAN_AFCAN_PRIVATE_CODE) Can_SleepMode
(P2CONST(Can_ControllerConfigType,AUTOMATIC,
                                          CAN_AFCAN_PRIVATE_CONST)LpController);

extern FUNC(Can_ReturnType, CAN_AFCAN_PRIVATE_CODE) Can_StartMode
(P2CONST(Can_ControllerConfigType,AUTOMATIC,
                                          CAN_AFCAN_PRIVATE_CONST)LpController);

extern FUNC(Can_ReturnType, CAN_AFCAN_PRIVATE_CODE) Can_StopMode
(P2CONST(Can_ControllerConfigType,AUTOMATIC,
                                          CAN_AFCAN_PRIVATE_CONST)LpController);

extern FUNC(Can_ReturnType, CAN_AFCAN_PRIVATE_CODE) Can_WakeupMode
(P2CONST(Can_ControllerConfigType,AUTOMATIC,
                                          CAN_AFCAN_PRIVATE_CONST)LpController);

#define CAN_AFCAN_STOP_SEC_PRIVATE_CODE
#include "MemMap.h"

#define CAN_AFCAN_START_SEC_CONST_UNSPECIFIED
#include "MemMap.h"

extern CONST(Tdd_Can_AFCan_ModeFuncPtr,
                                  CAN_AFCAN_PRIVATE_CONST) Can_GstModeFuncPtr[];

#define CAN_AFCAN_STOP_SEC_CONST_UNSPECIFIED
#include "MemMap.h"
#endif /* CAN_MODECNTRL_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
