/**
	******************************************************************************
	* @file           : emt_io.h
	* @brief          :
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_IO_H__
#define __EMT_IO_H__

/* Includes ------------------------------------------------------------------*/
#include "emt_defines.h"

#if (__TARGET_PROJECT__ == __ZDR065__)
#include "emt_io_zdr065.h"
#elif (__TARGET_PROJECT__ == __ZDR_D770R__)
#include "emt_io_zdr-d770r.h"
#elif (__TARGET_PROJECT__ == __ZDR027__)
#include <emt_io_zdr027.h>
#elif (__TARGET_PROJECT__ == __LXQ2000__)
#include "emt_io_lxq2000.h"
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__)
#include "emt_io_lxq2000_cr3.h"
#elif (__TARGET_PROJECT__ == __PRIME__)
#include "emt_io_prime.h"
#elif (__TARGET_PROJECT__ == __MAGNUS__)
#include <emt_io_magnus.h>
#elif (__TARGET_PROJECT__ == __TRINITY__)
#include <emt_io_trinity.h>
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
#include <emt_io_cyclops.h>
#endif

#endif /* __EMT_IO__ */
