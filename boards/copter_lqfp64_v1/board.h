/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the Olimex STM32-LCD proto board.
 */

/*
 * Board identifier.
 */
#define BOARD_COPTER_LQFP64_V1
#define BOARD_NAME              "Copter LQFP64 v1"

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F10X_HD
#define STM32F1XX

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
 * Everything input with pull-up except:
 * PA0  - tim2: ch1
 * PA1  - tim2: ch2
 * PA2  - tim2: ch3
 * PA3  - tim2: ch4
 * PA4  - lcd ce
 * PA5  - lcd rs/dc
 * PA6  - lcd rst
 * PA7  - lcd pwm
 * PA8  - rfm12b rst
 * PA9  - rfm12b irq
 */
#define VAL_GPIOACRL            0x3333BBBB      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x88888883      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB3  - spi: spi1 sck (remap)
 * PB4  - spi: spi1 miso (remap)
 * PB5  - spi: spi1 mosi (remap)
 * PB6  - gps: usart1 tx (remap)
 * PB7  - gps: usart1 rx (remap)
 * PB10 - imu: i2c2 scl
 * PB11 - imu: i2c2 sda
 * PB12 - rfm12b cs
 * PB13 - rfm12b clk (spi2)
 * PB14 - rfm12b miso (spi2)
 * PB15 - rfm12b mosi (spi2)
 */
#define VAL_GPIOBCRL            0xBBBBB788      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0xBBB3EE88      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

// 
#define SYS_LED_PORT            GPIOB
#define SYS_LED_PIN             2

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC0  - adc3: in0
 * PC1  - adc3: in1
 * PC2  - adc3: in2
 * PC3  - adc3: in3
 * PC10 - bt: usart3 tx (remap)
 * PC11 - bt: usart3 rx (remap)
 * PC13 - joy left
 * PC14 - joy right
 * PC15 - joy enter
 */
#define VAL_GPIOCCRL            0x88880000      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x8888BB88	    /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * PD0  - joy up
 * PD1  - joy down
 * Everything input with pull-up except:
 */
#define VAL_GPIODCRL            0x88888888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * Port F setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOFCRL            0x88888888      /*  PF7...PF0 */
#define VAL_GPIOFCRH            0x88888888      /* PF15...PF8 */
#define VAL_GPIOFODR            0xFFFFFFFF

/*
 * Port G setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOGCRL            0x88888888      /*  PG7...PG0 */
#define VAL_GPIOGCRH            0x88888888      /* PG15...PG8 */
#define VAL_GPIOGODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palClearPad(GPIOD, GPIOD_USB_DISC)

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) palSetPad(GPIOD, GPIOD_USB_DISC)

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
