#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

/* Module to initialize and cleanup the light sensor thread.
Supports updating (repeatedly reading in_voltage1_raw) and getting a voltage from the light sensor
in its digital form (e.g. 4095) or real world analog form (e.g. 1.2V).
*/

#include <stdint.h>

void *updateVoltage1Reading(void *args);
int getVoltage1Reading();
double getRealWorldVoltage1();
void createLightSensorThread();
void joinLightSensorThread();
void shutdownLightSensor();

#endif