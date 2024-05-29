#include "sampler.h"
#include "hal/display.h"
#include "hal/lightsensor.h"
#include "hal/pwm_led.h"
#include "hal/potentiometer.h"
#include "hal/general_command.h"
#include "period_timer.h"

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define A2D_VOLTAGE_REF_V  1.8 
#define A2D_MAX_READING    4095
#define HZ_CONVERSION     40
#define SAMPLES_PER_SECOND 20

static pthread_t printingThread;
static bool shutdown = false;

static void printSample(int dips){
    int sample = getCurrentSize();
    int potentiometer[2] = {getVoltage0Reading(), getVoltage0Reading() / HZ_CONVERSION};
    double averageSample = getAverageVoltage();

    printf("Smpl/s = %4d\tPOT @ %4d => %4dHz\tavg = %.3fV\tdips = %4d", sample, potentiometer[0],
     potentiometer[1], averageSample, dips);
}

static void printTimingJitter(){
    Period_statistics_t pStats;
    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);
    printf("\tSmpl ms[%6.3f, %6.3f] avg %.3f/%d\n", pStats.minPeriodInMs, pStats.maxPeriodInMs,
     pStats.avgPeriodInMs, pStats.numSamples);    
}

// Line 2: print 20 samples from the previous second
static void printRecentSamples(){
    int totalSamples = getCurrentSize();

    double *history = getHistoryBuffer();
    printf("  ");
    
    if (totalSamples < SAMPLES_PER_SECOND) {
        for (int i = 0; i < totalSamples; i++) {
            printf("%d:%.3f  ", i, history[i]);
        }
    }
    else {
        int step = totalSamples / SAMPLES_PER_SECOND;
        for (int i = 0; i < totalSamples; i += step) {
            printf("%d:%.3f  ", i, history[i]);
        }
    }
    printf("\n");
}

void *printing(void *args){
    (void)args;
    
    sleepForMs(1100); // skip first print because no history yet

    pthread_mutex_t* mutex = get_mutex();

    while(!shutdown){
        
        int dips = 0;

        pthread_mutex_lock(mutex);

        moveCurrentDataToHistory();

        pthread_mutex_unlock(mutex);
        
        dips = analyzeLightDips();
        setHistoryDips(dips);

        pthread_mutex_lock(mutex);

        setDisplay(dips);
        printSample(dips);
        printTimingJitter();
        printRecentSamples();
        setHistorySize(getCurrentSize());
        setCurrentSize(0);
        
        pthread_mutex_unlock(mutex);

        sleepForMs(1000);
    } 

    return NULL;
}

void createPrintingThread(){
    pthread_create(&printingThread, NULL, printing, NULL);
}

void joinPrintingThread(){
    pthread_join(printingThread, NULL);
}

void shutdownPrintingThread(){
    shutdown = true;
}