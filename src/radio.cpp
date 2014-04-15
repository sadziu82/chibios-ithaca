#include <ithaca.hpp>

#if ITHACA_USE_RADIO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

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
RadioPacket::RadioPacket(Type type, uint8_t group_id) {
    this->_type = type;
    this->_group_id = group_id;
    this->_payload_length = 0;
    this->_payload = NULL;
    //
    consoleDebug("RadioPacket() created\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
RadioPacket::RadioPacket(uint8_t *raw_data) {
    //
    this->_payload_length = 0;
    this->_payload = NULL;
    //
    this->setRawData(raw_data);
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setPayloadLength(uint8_t length) {
    //
    if (this->_payload != NULL) {
        // release memory
        chHeapFree(this->_payload);
        //
        this->_payload_length = 0;
        this->_payload = NULL;
    }
    // try to allocate memory
    this->_payload = (uint8_t *)chHeapAlloc(NULL, length);
    // set correct length if memory allocated
    if (this->_payload != NULL) {
        this->_payload_length = length;
        // fill with 0x55 by default
        memset(this->_payload, 0x55, this->_payload_length);
        consoleDebug("setPayloadLength() allocated _payload buffer, length: %d\r\n", this->_payload_length);
    } else {
        consoleDebug("setPayloadLength() failed\r\n");
    }
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::getRawData(uint8_t *raw_data) {
    // fill header
    raw_data[0] = (static_cast<uint8_t>(this->_type) << 6) | this->_group_id;
    raw_data[1] = this->_payload_length;
    // copy raw data
    memcpy(raw_data + 2, this->_payload, this->_payload_length);
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setRawData(uint8_t *raw_data) {
    //
    this->_type = static_cast<RadioPacket::Type>((raw_data[0] & 0xC0) >> 6);
    this->_group_id = raw_data[0] & 0x3F;
    // allocate buffer
    this->setPayloadLength(raw_data[1] & 0x3F);
    // copy raw data
    if (this->_payload_length > 0) {
        memcpy(this->_payload, raw_data + 2, this->_payload_length);
    }
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t &RadioPacket::operator[](uint8_t idx) {
    //
    if (idx >=this->_payload_length) {
        return this->_payload_trash;
    } else {
        return this->_payload[idx];
    }
}

/*
 * @brief   ...
 * @details ...
 */
RadioRFM12B::RadioRFM12B(void) {
    //
    this->_busy.name = "RadioPacket";
    this->_busy.flag = false;
}

#endif /* ITHACA_USE_RADIO */

