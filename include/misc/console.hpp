#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include "string.h"
#include "chsprintf.h"

/*                                                                           
 * @brief   console ...
 */                                                                          
class Console {
public:
    Console(void);
    void init(SerialDriver *drv);
    void write(char *fmt, ...);
protected:
private:
    const uint8_t _buffer_size = 0x80;
    const SerialConfig _cfg = {115200, 0, 0, 0};
    SerialDriver *_drv;
    Mutex _mutex;
};

/*                                                                           
 * @brief   console ...
 */                                                                          
extern Console console;

#endif /* _CONSOLE_HPP_ */

