/*============================================================================*/
/* Project      = AUTOSAR NEC MCAL Components                                 */
/* Module       = SchM_Lin.h                                                  */
/* Version      = 3.0.0                                                       */
/* Date         = 17.12.2008                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2008 by NEC Electronics (Europe) GmbH,                       */
/*               a company of the NEC Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for SchM Component                                     */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* NEC Electronics (Europe) GmbH the following shall apply!                   */
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
/*              Compiler:       GHS V4.2.3                                    */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V3.0.0:  17.12.2008   : Initial Version
*/
/******************************************************************************/
#ifndef SCHM_LIN_H
#define SCHM_LIN_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ComStack_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* File version Information to perform the version check */

#define SCHM_LIN_AR_MAJOR_VERSION                          1
#define SCHM_LIN_AR_MINOR_VERSION                          1
#define SCHM_LIN_AR_PATCH_VERSION                          0
  
#define SCHM_LIN_SW_MAJOR_VERSION                          3
#define SCHM_LIN_SW_MINOR_VERSION                          0
#define SCHM_LIN_SW_PATCH_VERSION                          0

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#define SchM_Enter_Lin(ExclusiveArea) SchM_Enter_Lin_## ExclusiveArea 
#define SchM_Exit_Lin(ExclusiveArea) SchM_Exit_Lin_## ExclusiveArea

#define SchM_Enter_Lin_LIN_RESPONSE_READY_PROTECTION          __asm("di")
#define SchM_Exit_Lin_LIN_RESPONSE_READY_PROTECTION            __asm("ei")

#define SchM_Enter_Lin_LIN_RESPONSE_READY_PROTECTION          __asm("di")
#define SchM_Exit_Lin_LIN_RESPONSE_READY_PROTECTION            __asm("ei")

#define SchM_Enter_Lin_LIN_DMA_PROTECTION                      __asm("di")
#define SchM_Exit_Lin_LIN_DMA_PROTECTION                       __asm("ei")

#define SchM_Enter_Lin_LIN_EXT_INT_EDGE_REG_PROTECTION          __asm("di")
#define SchM_Exit_Lin_LIN_EXT_INT_EDGE_REG_PROTECTION            __asm("ei")

#endif /* SCHM_LIN_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
