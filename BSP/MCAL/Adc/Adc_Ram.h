/*============================================================================*/
/* Project      = AUTOSAR Renesas Xx4 MCAL Components                         */
/* Module       = Adc_Ram.h                                                   */
/* Version      = 3.1.3                                                       */
/* Date         = 20-Mar-2013                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2009-2013 by Renesas Electronics Corporation                 */
/*============================================================================*/
/* Purpose:                                                                   */
/* Global variable declarations.                                              */
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
 * V3.0.0:  10-Jul-2009  : Initial version
 *
 * V3.0.1:  12-Oct-2009  : As per SCR 052, the following changes are made
 *                         1. Extern declaration of the array
 *                            Adc_GaaOperationClrMask, Adc_GaaExtTrigClrMask
 *                            are removed.
 *                         2. Extern declaration of the array
 *                            Adc_GaaOperationMask is added.
 *
 * V3.0.2:  02-Dec-2009  : As per SCR 157, the following changes are made
 *                         1. Adc_GpRunTimeData declaration is changed.
 *                         2. Adc_GucPopFrmQueue is put within pre-compile
 *                            option.
 *                         3. Adc_GpDmaHWUnitMapping, Adc_GpDmaCGUnitMapping
 *                            are added.
 *
 * V3.0.3:  05-Jan-2010  : As per SCR 179, Adc_GucResultRead and
 *                         Adc_GblSampleComp are deleted.
 *
 * V3.0.4:  01-Jul-2010  : As per SCR 295, Adc_GaaStreamEnableMask[],
 *                         Adc_GaaOperationMask[] and
 *                         Adc_GaaCGmConvStatusMask[] are moved to individual
 *                         files where they are used.
 *
 * V3.1.0:	27-Jul-2011  : Modified for DK-4
 * V3.1.1:  14-Feb-2012  : Merged the fixes done for Fx4L Adc driver
 *
 * V3.1.2:  04-Jun-2012  : As per SCR 019, the following changes are made
 *                         1. Compiler version is removed from environment 
 *                            section.
 *                         2. File version is changed.
 * V3.1.3:  20-Mar-2013  : As per SCR 083, the following changes are made
 *                         1. "Adc_GaaHwUnitStatus" is added.
 */
/******************************************************************************/

#ifndef ADC_RAM_H
#define ADC_RAM_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*
 * AUTOSAR specification version information
 */
#define ADC_RAM_AR_MAJOR_VERSION    ADC_AR_MAJOR_VERSION_VALUE
#define ADC_RAM_AR_MINOR_VERSION    ADC_AR_MINOR_VERSION_VALUE
#define ADC_RAM_AR_PATCH_VERSION    ADC_AR_PATCH_VERSION_VALUE

/*
 * File version information
 */
#define ADC_RAM_SW_MAJOR_VERSION    3
#define ADC_RAM_SW_MINOR_VERSION    1
#define ADC_RAM_SW_PATCH_VERSION    2

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

#if (ADC_SW_MAJOR_VERSION != ADC_RAM_SW_MAJOR_VERSION)
#error "Software major version of Adc.h and Adc_Ram.h did not match!"
#endif
#if (ADC_SW_MINOR_VERSION!= ADC_RAM_SW_MINOR_VERSION)
#error "Software minor version of Adc.h and Adc_Ram.h did not match!"
#endif

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

#define ADC_START_SEC_VAR_NOINIT_UNSPECIFIED
#include "MemMap.h"

/* Global database pointer */
extern P2CONST(Adc_ConfigType, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpConfigPtr;

/* Global pointer variable for hardware unit configuration */
extern P2CONST(Tdd_Adc_HwUnitConfigType, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpHwUnitConfig;

/* Global pointer variable for group configuration */
extern P2CONST(Tdd_Adc_GroupConfigType, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpGroupConfig;

#if (ADC_HW_TRIGGER_API == STD_ON)
/* Global pointer variable for HW group configuration */
extern P2CONST(Tdd_Adc_HWGroupTriggType, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpHWGroupTrigg;
#endif

#if (ADC_DMA_MODE_ENABLE == STD_ON)
/* Global pointer variable for HW group configuration */
extern P2CONST(Tdd_Adc_DmaUnitConfig, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpDmaUnitConfig;
/* Global pointer to DMA HW unit array mapping */
extern P2CONST(Adc_HwUnitType, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpDmaHWUnitMapping;
#if ((ADC_PRIORITY_IMPLEMENTATION == ADC_PRIORITY_HW) || \
         (ADC_PRIORITY_IMPLEMENTATION == ADC_PRIORITY_HW_SW))
/* Global pointer to DMA CG unit array mapping */
extern P2CONST(uint8, ADC_CONST, ADC_CONFIG_CONST)
Adc_GpDmaCGUnitMapping;
#endif
#endif /* #if (ADC_DMA_MODE_ENABLE == STD_ON) */

/* Global pointer variable for group ram data */
extern P2VAR(Tdd_Adc_ChannelGroupRamData, ADC_NOINIT_DATA, ADC_CONFIG_DATA)
Adc_GpGroupRamData;

/* Global pointer variable for hardware unit ram data */
extern P2VAR(Tdd_Adc_HwUnitRamData, ADC_NOINIT_DATA, ADC_CONFIG_DATA)
Adc_GpHwUnitRamData;

/* Global pointer variable for runtime ram data */
extern P2VAR(Tdd_Adc_RunTimeData, ADC_NOINIT_DATA, ADC_CONFIG_DATA)
Adc_GpRunTimeData;

/* Indicates max no of SW triggered groups configured */
extern VAR(boolean, ADC_NOINIT_DATA) Adc_GaaHwUnitStatus[];

#define ADC_STOP_SEC_VAR_NOINIT_UNSPECIFIED
#include "MemMap.h"

#define ADC_START_SEC_VAR_NOINIT_8BIT
#include "MemMap.h"

#if ((ADC_PRIORITY_IMPLEMENTATION == ADC_PRIORITY_SW) || \
               (ADC_PRIORITY_IMPLEMENTATION == ADC_PRIORITY_HW_SW))
/* Indicates the group is popped out of the queue */
extern VAR(uint8, ADC_NOINIT_DATA) Adc_GucPopFrmQueue;
#endif

/* Indicates max no of SW triggered groups configured */
extern VAR(uint8, ADC_NOINIT_DATA) Adc_GucMaxSwTriggGroups;

#if (ADC_DMA_MODE_ENABLE == STD_ON)
/* Indicates no of DMA channel Ids configured */
extern VAR(uint8, ADC_NOINIT_DATA) Adc_GucMaxDmaChannels;
#endif

#define ADC_STOP_SEC_VAR_NOINIT_8BIT
#include "MemMap.h"

#define ADC_START_SEC_VAR_1BIT
#include "MemMap.h"

#if  (ADC_DEV_ERROR_DETECT == STD_ON)
/* Holds the status of Initialisation */
extern VAR(boolean, ADC_INIT_DATA) Adc_GblDriverStatus;
#endif

#define ADC_STOP_SEC_VAR_1BIT
#include "MemMap.h"

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* ADC_RAM_H */

/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
