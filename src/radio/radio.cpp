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
uint8_t *RadioPacket::data(void) {
    return this->_data;
}

/*
 * @brief   radio ....
 */
uint8_t RadioPacket::len(void) {
    return this->_len;
}

/*
 * @brief   radio ....
 */
void RadioPacket::inc(void) {
    this->_data[0]++;
}

/*
 * @brief   radio ....
 */
uint8_t RadioPacket::data0(void) {
    return this->_data[0];
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

