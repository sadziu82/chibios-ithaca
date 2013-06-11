/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

// Board identifier.
#define BOARD_MCU48
#define BOARD_NAME              "mcu48"

// Board frequencies.
#define STM32_HSICLK            8000000

// MCU type
#define STM32F10X_MD

// memory
#define MEM_CE_PORT            GPIOB
#define MEM_CE_PIN             2

// sys led
#define SYS_LED_PORT            GPIOB
#define SYS_LED_PIN             3

// usb
#define USB_DP_PORT             GPIOA
#define USB_DP_PIN              11
#define USB_DM_PORT             GPIOA
#define USB_DM_PIN              12
#define USB_DISC_PORT           GPIOA
#define USB_DISC_PIN            15

// sonar
#define SONAR_IN1_PORT          GPIOB
#define SONAR_IN1_PIN           12
#define SONAR_IN2_PORT          GPIOB
#define SONAR_IN2_PIN           13
#define SONAR_OUT1_PORT         GPIOB
#define SONAR_OUT1_PIN          14
#define SONAR_OUT2_PORT         GPIOB
#define SONAR_OUT2_PIN          15


/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 */
#define VAL_GPIOACRL            0xBBB33888      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x388FF338      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
    PA4  - rfm12b ce
    PA5  - rfm12b clk
    PA6  - rfm12b miso
    PA7  - rfm12b mosi
    PA9  - lcd rst
    PA10 - lcs data/command
    PA11 - usb d+
    PA12 - usb d-
    PA15 - usb disc
*/

/*
 * Port B setup.
 */
#define VAL_GPIOBCRL            0x44883888      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0xBBB38888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
    PB2  - mem ce
    PB3  - sys led
    PB4  - tim3 ch1
    PB5  - tim3 ch2
    PB6  - tim4 ch1
    PB7  - tim4 ch2
    PB12 - lcd ce
    PB13 - lcd clk
    PB14 - lcd miso
    PB15 - lcd mosi (mosi/miso in bidi-mode)
*/

/*
 * Port C setup.
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x83888888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
    PC13 - rfm12b irq
    PC14 - rfm12b rst
 */

/*
 * Port D setup.
 */
#define VAL_GPIODCRL            0x88888888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF


/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palClearPad( USB_DISC_PORT, USB_DISC_PIN )

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) palSetPad( USB_DISC_PORT, USB_DISC_PIN )

/* */
#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
