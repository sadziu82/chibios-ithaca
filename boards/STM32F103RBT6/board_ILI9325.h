/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9325/board_ILI9325_template.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9325 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

/*
 * @brief   Display ports
 */
#define SET_CS		palSetPad(GPIOC, 8)
#define CLR_CS		palClearPad(GPIOC, 8)

#define SET_RS		palSetPad(GPIOC, 9)
#define CLR_RS		palClearPad(GPIOC, 9)

#define SET_WR		palSetPad(GPIOC, 10)
#define CLR_WR		palClearPad(GPIOC, 10)

#define SET_RD		palSetPad(GPIOC, 11)
#define CLR_RD		palClearPad(GPIOC, 11)


/**
 * @brief   Initialise the board for the display.
 *
 * @param[in] g			The GDisplay structure
 *
 * @note	Set the g->board member to whatever is appropriate. For multiple
 * 			displays this might be a pointer to the appropriate register set.
 *
 * @notapi
 */
static inline void init_board(GDisplay *g) {
	(void) g;
    g->board = 0;
    // data bus
    palSetGroupMode(GPIOC, 0x00FF, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetGroupMode(GPIOB, 0xFF00, 0, PAL_MODE_OUTPUT_PUSHPULL);
    // control lines
    palSetPadMode(GPIOC, 8, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, 9, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, 10, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, 11, PAL_MODE_OUTPUT_PUSHPULL);
    // Configure the pins to a well know state
    SET_RS;
    SET_RD;
    SET_WR;
}

/**
 * @brief   After the initialisation.
 *
 * @param[in] g			The GDisplay structure
 *
 * @notapi
 */
static inline void post_init_board(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] g			The GDisplay structure
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 *
 * @notapi
 */
static inline void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
    (void)state;
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] percent		0 to 100%
 *
 * @notapi
 */
static inline void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void acquire_bus(GDisplay *g) {
	(void) g;
    CLR_CS;
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void release_bus(GDisplay *g) {
	(void) g;
    SET_CS;
}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] index			The index register to set
 *
 * @notapi
 */
static inline void write_index(GDisplay *g, uint16_t index) {
	(void) g;
    //
    palWritePort(GPIOC,(palReadLatch(GPIOC)&0xFF00)|(index&0x00FF));
    palWritePort(GPIOB,(palReadLatch(GPIOB)&0x00FF)|(index&0xFF00));
    CLR_RS; CLR_WR; SET_WR; SET_RS;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] data			The data to send
 *
 * @notapi
 */
static inline void write_data(GDisplay *g, uint16_t data) {
	(void) g;
    palWritePort(GPIOC,(palReadLatch(GPIOC)&0xFF00)|(data&0x00FF));
    palWritePort(GPIOB,(palReadLatch(GPIOB)&0x00FF)|(data&0xFF00));
    CLR_WR; SET_WR;
}

/**
 * @brief   Set the bus in read mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void setreadmode(GDisplay *g) {
	(void) g;
    palSetGroupMode(GPIOC, 0x00FF, 0, PAL_MODE_INPUT);
    palSetGroupMode(GPIOB, 0xFF00, 0, PAL_MODE_INPUT);
}

/**
 * @brief   Set the bus back into write mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void setwritemode(GDisplay *g) {
	(void) g;
    palSetGroupMode(GPIOC, 0x00FF, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetGroupMode(GPIOB, 0xFF00, 0, PAL_MODE_OUTPUT_PUSHPULL);
}

/**
 * @brief   Read data from the lcd.
 * @return	The data from the lcd
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline uint16_t read_data(GDisplay *g) {
    uint16_t value;
	(void) g;
    CLR_RD;
    value = (palReadPort(GPIOC)&0x00FF)|(palReadPort(GPIOB)&0xFF00);
    SET_RD;
    return value;
}

#endif /* GDISP_LLD_BOARD_H */
/** @} */

