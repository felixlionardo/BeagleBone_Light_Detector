#include "hal/pwm_led.h"
#include "hal/general_command.h"
#include "hal/potentiometer.h"
#include "hal/lightsensor.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#define PWM_LED_PATH "/dev/bone/pwm/0/b"
#define HZ_CONVERSION 40

static bool shutdown = false;
static pthread_t pwmLEDThread;
static int previousHz = 0;

void initializePWMLED(){
    runCommand("config-pin P9_21 pwm");
    writeToFile(PWM_LED_PATH "/enable", "0");
    writeToFile(PWM_LED_PATH "/enable", "1");

}

void *setPWMLED(void *args){
    (void)args;
    
    while(!shutdown){
        writeToFile(PWM_LED_PATH "/enable", "1");
        int rawValue = getVoltage0Reading();
        int hz = rawValue/ HZ_CONVERSION;
        if(hz == 0){
            writeToFile(PWM_LED_PATH "/enable", "0");
            sleepForMs(100);
            continue;
        }
        else if(previousHz == hz){
            sleepForMs(100);
            continue;
        }

        else{

            if(hz < 1){
                hz = 1;
            }

            writeToFile(PWM_LED_PATH "/enable", "1");

            previousHz = hz;

            int period = 1000000000 / hz;
            int duty_cycle = period / 2;

            char periodStr[20];
            char dutyStr[20];

            sprintf(periodStr, "%d", period);
            sprintf(dutyStr, "%d", duty_cycle);

            writeToFile(PWM_LED_PATH "/duty_cycle", "0");
            writeToFile(PWM_LED_PATH "/period", periodStr);
            writeToFile(PWM_LED_PATH "/duty_cycle", dutyStr);
            sleepForMs(100);
        }
    }
    writeToFile(PWM_LED_PATH "/enable", "0");
    return NULL;
}

void createPWMLEDThread(){
    pthread_create(&pwmLEDThread, NULL, setPWMLED, NULL);
}

void joinPWMLEDThread(){
    pthread_join(pwmLEDThread, NULL);
}

void shutdownPWMLED(){
    shutdown = true;
}