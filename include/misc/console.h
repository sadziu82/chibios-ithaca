#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string.h>

#if ITHACA_USE_CONSOLE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    SerialDriver *serial_driver;
    SerialConfig serial_config;
    // 
    Mutex mutex;
} console_t;
 
/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
#if ITHACA_USE_CONSOLE_DEVEL
#define consoleDevel(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#else
#define consoleDevel(fmt, ...)
#endif

/*
 * @brief   ...
 * @details ...
 */
#if ITHACA_USE_CONSOLE_DEBUG
#define consoleDebug(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#else
#define consoleDebug(fmt, ...)
#endif

/*
 * @brief   ...
 * @details ...
 */
#if ITHACA_USE_CONSOLE_INFO
#define consoleInfo(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#else
#define consoleInfo(fmt, ...)
#endif

/*
 * @brief   ...
 * @details ...
 */
#if ITHACA_USE_CONSOLE_WARN
#define consoleWarn(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#else
#define consoleWarn(fmt, ...)
#endif

/*
 * @brief   ...
 * @details ...
 */
#if ITHACA_USE_CONSOLE_WARN
#define consoleError(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#else
#define consoleError(fmt, ...)
#endif


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    // 
    extern console_t console;
    // 
    bool consoleInit(console_t *console);
    bool consoleWrite(console_t *console, const char *text);
    bool consolePrintf(console_t *console, char *fmt, ...);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_CONSOLE */

#endif /* _CONSOLE_H_ */

