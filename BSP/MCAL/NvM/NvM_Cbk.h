/**
	\addtogroup MODULE_NvM NvM
	Non-volatile Memory Management Module.
*/
/*@{*/
/***************************************************************************//**
	\file		NvM_Cfg.h
	\brief		Callback of NVRAM Management Module.
	\author		zhaoyg
	\version	1.0
	\date		2012-3-27
	\warning	Copyright (C), 2012, PATAC.
*//*----------------------------------------------------------------------------
	\history
	<No.>	<author>	<time>		<version>	<description>
	1		zhaoyg		2012-3-27	1.0			Create
*******************************************************************************/
#ifndef _NVM_CBK_H
#define _NVM_CBK_H

/*******************************************************************************
	Include Files
*******************************************************************************/

/*******************************************************************************
	Macro Definition
*******************************************************************************/

/*******************************************************************************
	Type Definition
*******************************************************************************/

/*******************************************************************************
	Prototype  Declaration
*******************************************************************************/
extern void NvM_JobEndNotification(void);
extern void NvM_JobErrorNotification(void);

#endif /* #ifndef _NVM_CBK_H */
/*@}*/
