#ifndef _POTENTIOMETER_H_
#define _POTENTIOMETER_H_

/* Module to initialize and cleanup the potentiometer thread.
Supports updating (repeatedly reading in_voltage0_raw) and getting a voltage from the potentiometer
in its digital form (e.g. 4095) or real world analog form (e.g. 1.2V).
*/

#include <stdint.h>

void *updateVoltage0Reading(void *args);
int getVoltage0Reading();
double getRealWorldVoltage0();
void createPotentiometerThread();
void joinPotentiometerThread();
void shutdownPotentiometer();

#endif