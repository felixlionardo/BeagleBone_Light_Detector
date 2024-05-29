#ifndef _PWN_LED_H_
#define _PWN_LED_H_

/* Module to initialize and cleanup the PWMLED thread.
Supports controlling the LED with the potentiometer. Repeatedly writes the period and duty_cycle parameters of the LED
based on the voltage read from the potentiometer. 
*/

#include <stdint.h>

void initializePWMLED();
void createPWMLEDThread();
void joinPWMLEDThread();
void shutdownPWMLED();

#endif