#include <ithaca.h>

#if ITHACA_USE_CONSOLE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

// 
#define CONSOLE_BUFFER_SIZE 0x80

// 
char buffer[CONSOLE_BUFFER_SIZE];

/*
 * @brief   working area for pca9633Player threads
 * @details serial_driver must be set before using console
 */
console_t console = {
    .serial_driver = NULL,
    .serial_config = {921600, 0, 0, 0},
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool consoleInit(console_t *console) {
    // 
    if (console->serial_driver == NULL) {
        return false;
    }
    //
    chMtxInit(&console->mutex);
    //
    sdStart(console->serial_driver, &console->serial_config);
    //
    return true;
}

/*
 * @brief   console ...
 * @details ...
 */
bool consoleWrite(console_t *console, const char *text) {
    // 
    consolePrintf(console, "%s", text);
    // 
    return true;
}

/*
 * @brief   console ...
 * @details ...
 */
bool consolePrintf(console_t *console, char *fmt, ...) {
    // 
    uint8_t len;
    va_list args;
    // 
    if (console->serial_driver == NULL) {
        return true;
    }
    // 
    chMtxLock(&console->mutex);
    //
    len = chsprintf(buffer, "[%ld] [%s] ", chTimeNow(), chThdSelf()->p_name);
    sdWrite(console->serial_driver, (uint8_t *)buffer, len);
    // 
    va_start(args, fmt);
    len = chvsprintf(buffer, fmt, args);
    va_end(args);
    // 
    sdWrite(console->serial_driver, (uint8_t *)buffer, len);
    //
    chMtxUnlock();
    // 
    return true;
}

#endif /* ITHACA_USE_CONSOLE */

