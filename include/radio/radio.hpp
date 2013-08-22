#ifndef _RADIO_H__
#define _RADIO_H__

/*
 * @brief   Radio library
 */
#include "ch.h"
#include "hal.h"
#include "math.h"
#include "string.h"
#include "chsprintf.h"
#include "console.hpp"

/*
 * @brief   Radio states ...
 */
typedef uint8_t radio_state_t;
typedef uint8_t radio_register_address_t;
typedef uint8_t radio_register_bits_t;
typedef uint8_t radio_command_t;
typedef uint16_t radio_node_address_t;

/*
 * @brief   radio ...
 */
const uint16_t RadioPacketSize = 0x20;

/*
 * @brief   Base display class
 */
class RadioPacket {
public:
    RadioPacket(void);
    uint8_t &operator[](uint8_t index);
protected:
    uint8_t _data[RadioPacketSize];
    uint8_t _dummy_data;
};

/*
 * @brief   Base display class
 */
class Radio {
public:
    Radio(void);
    virtual bool send(RadioPacket &packet, systime_t tmout_period) { return false; }; // TODO
    virtual bool recv(RadioPacket &packet, systime_t tmout_period) { return false; }; // TODO
    virtual bool ping(void) { return false; }; // TODO
    void setGroupId(uint8_t group);
    radio_state_t getState(void);
    class State {
    public:
        static const radio_state_t Uninit = 0x00;
        static const radio_state_t Present = 0x01;
        static const radio_state_t Ready = 0x02;
        static const radio_state_t Idle = 0x03;
        static const radio_state_t PreSent = 0x04;
        static const radio_state_t Sending = 0x05;
        static const radio_state_t PreRecv = 0x06;
        static const radio_state_t Receiving = 0x07;
        static const radio_state_t Error = 0xFF;
    };
    class NodeAddress {
    public:
        static const radio_node_address_t Default = 0x00;
        static const radio_node_address_t Broadcast = 0xFF;
    };
protected:
    radio_state_t setState(radio_state_t new_state);
    bool lock(void);
    bool unlock(void);
    radio_state_t _state;
    Mutex _state_mtx;
    uint8_t _group_id;
    uint8_t _src_id;
    uint8_t _dst_id;
};

#endif /* _RADIO_H__ */

