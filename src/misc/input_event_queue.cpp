#include <ithaca.hpp>

#if ITHACA_USE_INPUT_EVENT_QUEUE || defined(__DOXYGEN__)

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
InputEventQueue::InputEventQueue(void) {
    this->event_count = 0;
}

/*
 * @brief   ...
 * @details ...
 */
void InputEventQueue::pushEvent(InputEvent *event) {
    //
    if (this->event_count == this->max_events) {
        consoleDebug("event queue full\r\n");
        return;
    }
    //
    consoleDebug("adding event to queue: %d, %d\r\n", event->type, event->key);
    this->events[this->event_count++] = event;
    consoleDebug("event added into queue, length: %d\r\n", this->event_count);
}

/*
 * @brief   ...
 * @details ...
 */
InputEvent *InputEventQueue::popEvent(void) {
    //
    if (this->event_count == 0) {
        consoleDebug("queue empty\r\n");
        return NULL;
    }
    //
    InputEvent *event;
    uint8_t i;
    // allocate memory for longer list
    event = this->events[0];
    consoleDebug("removing event from queue: %d, %d\r\n", event->type, event->key);
    for (i = 1; i < this->event_count; i++) {
        this->events[i - 1] = this->events[i];
    }
    this->event_count--;
    consoleDebug("removed event from queue, length: %d\r\n", this->event_count);
    return event;
}

#endif /* ITHACA_USE_INPUT_EVENT_QUEUE */

