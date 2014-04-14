#ifndef _RADIO_HPP_
#define _RADIO_HPP_

#if ITHACA_USE_RADIO || defined(__DOXYGEN__)

/*

  Radio Data Frame format

+---------+--------+----------------------------+----------------------------+
| Field   | Lenght | Transmitter                | Copter                     |
+---------+--------+----------------------------+----------------------------+
| id      |      1 | 7-6: 0 - standalone, 1 - master, 2 - slave              |
|         |        | 5-0: group number                                       |
+---------+--------+----------------------------+----------------------------+
| length  |      1 | 7-6: unused                                             |
|         |        | 5-0: stream data length (0 for now, under development)  |
+---------+--------+----------------------------+----------------------------+ R
| control |      2 |    15: abort               |    15: low battery         | C
|         |        |    14: arm/disarm req.     |    14: armed/disarmed      |
|         |        |    13: return home req.    |    13: returning home      | P
|         |        | 12-10: fly mode            | 12-10: current fly mode    | a
|         |        |   9-0: unused for now      |     9: sensors ok          | c
|         |        |                            |     8: gps lock            | k
|         |        |                            |   7-0: battery voltage     | e
+---------+--------+----------------------------+----------------------------+ t
| rc data |      4 | 31-24: roll (right horiz.) | 31-24: roll (in degree)    |
|         |        | 23-16: pitch (right vert.) | 23-16: pitch (in degree)   | o
|         |        |  15-8: yaw (left horiz.)   |  15-0: yaw (in degree)     | n
|         |        |   7-0: thr. (left vert.)   |                            | l
+---------+--------+----------------------------+----------------------------+ y
| ....... | ...... | .......... stream not yet implemented ................. |
+---------+--------+----------------------------+----------------------------+
| crc32   |      4 |                          crc32                          |
+---------+--------+----------------------------+----------------------------+

fly mode:
  000 - easy fly (front on vector between current and start position)
  001 - easy fly with altitude hold
  010 - position hold
  011 - manual
  100 - auto pilot - follow path
  101 - ...
  110 - ...
  111 - ...

 */

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
class RadioPacket {
    public:
        enum class Type: uint8_t {
            Standalone,
            Master,
            Slave,
        };
    protected:
        //
        uint8_t _packet_length;
        // id field
        Type _type;
        uint8_t _group_id;
        // length field
        uint8_t _payload_length;
        uint8_t *_payload;
    public:
        RadioPacket(Type type, uint8_t group_id);
        RadioPacket(uint8_t *raw_data);
        void getRawData(uint8_t *raw_data);
        void setRawData(uint8_t *raw_data);
};

/*
 * @brief   ...
 * @details ...
 */
class Radio {
    protected:
    public:
        Radio(void);
        virtual init(void);
        virtual void sendPacket(RadioPacket *packet, systime_t tmout);
        virtual RadioPacket *recvPacket(systime_t tmout);
};

/*
 * @brief   ...
 * @details ...
 */
class RadioRFM12B: public Radio {
    protected:
        // busy flag
        ithaca_lock_t _busy;
    public:
        RadioRFM12B(void);
        virtual init(void);
        virtual void sendPacket(RadioPacket *packet, systime_t tmout);
        virtual RadioPacket *recvPacket(systime_t tmout);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_RADIO */

#endif /* _RADIO_HPP_ */

