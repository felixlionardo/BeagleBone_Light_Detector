// Main program to build the application

#include "hal/general_command.h"
#include "hal/display.h"
#include "hal/potentiometer.h"
#include "hal/lightsensor.h"
#include "hal/pwm_led.h"
#include "udp.h"
#include "period_timer.h"
#include "sampler.h"
#include "print_result.h"
#include "shutdown.h"

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define MSG_MAX_LEN 1024
#define PORT        12345

int main()
{
    createThreads();

    waitShutdown();

    joinThreads();
    
    return 0;
}