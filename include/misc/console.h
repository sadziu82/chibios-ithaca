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
//#define consoleDebug(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
#define consoleDebug(fmt, ...) 
#define consoleInfo(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
//#define consoleInfo(fmt, ...) 
#define consoleWarn(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
//define consoleWarn(fmt, ...) 
#define consoleError(fmt, ...) consolePrintf(&console, fmt, ##__VA_ARGS__)
//#define consoleError(fmt, ...) 

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

