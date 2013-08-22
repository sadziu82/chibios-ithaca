/*
 * @brief   ...
 */

#include "radio.hpp"

/*
 * @brief   radio ....
 */
RadioPacket::RadioPacket(void) {
}

/*
 * @brief   radio ....
 */
uint8_t &RadioPacket::operator[](uint8_t index) {
    if (index < RadioPacketSize) {
        return this->_data[index];
    } else {
        return this->_dummy_data;
    }
}

/*
 * @brief   radio ....
 */
Radio::Radio(void) {
    chMtxInit(&this->_state_mtx);
    // 
    this->setState(Radio::State::Uninit);
}

/*
 * @brief   radio ....
 */
void Radio::setGroupId(uint8_t group) {
    this->_group_id = group;
}

/*
 * @brief   radio ....
 */
radio_state_t Radio::getState(void) {
    return this->_state;
}

/*
 * @brief   radio ....
 */
radio_state_t Radio::setState(radio_state_t new_state) {
    radio_state_t old_state = this->_state;
    this->_state = new_state;
    return old_state;
}

/*
 * @brief   radio ....
 */
bool Radio::lock(void) {
    return chMtxTryLock(&this->_state_mtx);
}

/*
 * @brief   radio ....
 */
bool Radio::unlock(void) {
    return chMtxUnlock();
}

