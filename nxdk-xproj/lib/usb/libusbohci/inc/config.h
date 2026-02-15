/**************************************************************************//**
 * @file     config.h
 * @version  V1.00
 * @brief    This header file defines the configuration of USB Host library.
 * @note
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef  _USBH_CONFIG_H_
#define  _USBH_CONFIG_H_

#ifndef USBH_USE_EXTERNAL_CONFIG

/// @cond HIDDEN_SYMBOLS

#include "N9H30.h"
#include "sys.h"


/*----------------------------------------------------------------------------------------*/
/*   Hardware settings                                                                    */
/*----------------------------------------------------------------------------------------*/
#define HCLK_MHZ               300          /* used for loop-delay. must be larger than 
                                               true HCLK clock MHz                        */

#define NON_CACHE_MASK         (0x80000000)

#define ENABLE_OHCI_IRQ()      sysEnableInterrupt(OHCI_IRQn)
#define DISABLE_OHCI_IRQ()     sysDisableInterrupt(OHCI_IRQn)
#define IS_OHCI_IRQ_ENABLED()  ((inpw(REG_AIC_IMR)>>OHCI_IRQn) & 0x1)
#define ENABLE_EHCI_IRQ()      sysEnableInterrupt(EHCI_IRQn)
#define DISABLE_EHCI_IRQ()     sysDisableInterrupt(EHCI_IRQn)
#define IS_EHCI_IRQ_ENABLED()  ((inpw(REG_AIC_IMR)>>EHCI_IRQn) & 0x1)

#define ENABLE_OHCI                         /* Enable OHCI host controller                */
#define ENABLE_EHCI                         /* Enable EHCI host controller                */

#define EHCI_PORT_CNT          2            /* Number of EHCI roothub ports               */
#define OHCI_PORT_CNT          2            /* Number of OHCI roothub ports               */
//#define OHCI_PER_PORT_POWER               /* OHCI root hub per port powered             */

#define OHCI_ISO_DELAY         4            /* preserved number frames while scheduling 
                                               OHCI isochronous transfer                  */

#define EHCI_ISO_DELAY         2            /* preserved number of frames while 
                                               scheduling EHCI isochronous transfer       */

#define EHCI_ISO_RCLM_RANGE    32           /* When inspecting activated iTD/siTD, 
                                               unconditionally reclaim iTD/isTD scheduled
                                               in just elapsed EHCI_ISO_RCLM_RANGE ms.    */

#define MAX_DESC_BUFF_SIZE     4096         /* To hold the configuration descriptor, USB 
                                               core will allocate a buffer with this size
                                               for each connected device. USB core does 
                                               not release it until device disconnected.  */

/*----------------------------------------------------------------------------------------*/
/*   Memory allocation settings                                                           */
/*----------------------------------------------------------------------------------------*/

#define STATIC_MEMORY_ALLOC    0       /* pre-allocate static memory blocks. No dynamic memory aloocation.
                                          But the maximum number of connected devices and transfers are
                                          limited.  */

#define MAX_UDEV_DRIVER        8       /*!< Maximum number of registered drivers                      */
#define MAX_ALT_PER_IFACE      12      /*!< maximum number of alternative interfaces per interface    */
#define MAX_EP_PER_IFACE       8       /*!< maximum number of endpoints per interface                 */
#define MAX_HUB_DEVICE         8       /*!< Maximum number of hub devices                             */

/* Host controller hardware transfer descriptors memory pool. ED/TD/ITD of OHCI and QH/QTD of EHCI
   are all allocated from this pool. Allocated unit size is determined by MEM_POOL_UNIT_SIZE.
   May allocate one or more units depend on hardware descriptor type.                                 */

#define MEM_POOL_UNIT_SIZE     128     /*!< A fixed hard coding setting. Do not change it!            */
#define MEM_POOL_UNIT_NUM      256     /*!< Increase this or heap size if memory allocate failed.     */

/*----------------------------------------------------------------------------------------*/
/*   Re-defined staff for various compiler                                                */
/*----------------------------------------------------------------------------------------*/
#ifdef __ICCARM__
#define   __inline    inline
#endif


/*----------------------------------------------------------------------------------------*/
/*   Debug settings                                                                       */
/*----------------------------------------------------------------------------------------*/
#define ENABLE_ERROR_MSG                    /* enable debug messages                      */
#define ENABLE_DEBUG_MSG                    /* enable debug messages                      */
//#define ENABLE_VERBOSE_DEBUG              /* verbos debug messages                      */
//#define DUMP_DESCRIPTOR                   /* dump descriptors                           */

#ifdef ENABLE_ERROR_MSG
#define USB_error            sysprintf
#else
#define USB_error(...)
#endif

#ifdef ENABLE_DEBUG_MSG
#define USB_debug            sysprintf
#ifdef ENABLE_VERBOSE_DEBUG
#define USB_vdebug         sysprintf
#else
#define USB_vdebug(...)
#endif
#else
#define USB_debug(...)
#define USB_vdebug(...)
#endif

#define USBH                 ((USBH_T *)0xB0007000)
#define HSUSBH               ((HSUSBH_T *)0xB0005000)


/// @endcond /*HIDDEN_SYMBOLS*/

#endif  /* USBH_USE_EXTERNAL_CONFIG */
#endif  /* _USBH_CONFIG_H_ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

