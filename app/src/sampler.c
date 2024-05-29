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
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 2000
#define DATA_WEIGHT 0.001
#define DIP_THRESHOLD 0.1
#define HYSTERESIS 0.03

static pthread_t sampleThread;

static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

static bool shutdown = false;

static int currentSize = 0;
static int historySize = 0;
static long long samplesTaken = 0;
static double averageVoltage = 0.0;
static int historyDips = 0;
static double currentBuffer[BUFFER_SIZE];
static double historyBuffer[BUFFER_SIZE];

pthread_mutex_t* get_mutex() {
    return &data_mutex;
}
void setCurrentSize(int size){
    currentSize = size;
}
void setHistorySize(int size){
    historySize = size;
}
void setHistoryDips(int d){
    historyDips = d;
}

int getCurrentSize(){
    return currentSize;
}
int getHistorySize(){
    return historySize;
}
int getHistoryDips(){
    return historyDips;
}

long long getSamplesTaken(){
    return samplesTaken;
}

double getAverageVoltage(){
    return averageVoltage;
}

double *getHistoryBuffer(){
    return historyBuffer;
}

void moveCurrentDataToHistory() {
    // Move current samples to history
     for (int i = 0; i < currentSize; ++i) {
        historyBuffer[i] = currentBuffer[i];
    }
}

int analyzeLightDips() {

    pthread_mutex_lock(&data_mutex);

    double* historySamples = historyBuffer;
    int size = currentSize;
    double currentAverage = averageVoltage;

    pthread_mutex_unlock(&data_mutex);

    int dipCount = 0;

    // Define parameters for detecting dips
    double dipThreshold = DIP_THRESHOLD;
    double hysteresis = HYSTERESIS;
    double dipTriggerThreshold = currentAverage - dipThreshold;
    double dipRecoveryThreshold = currentAverage - hysteresis;

    bool dipInProgress = false;

    // Analyze history for dips
    for (int i = 0; i < size; ++i) {

        if (historySamples[i] <= dipTriggerThreshold) {

            if (!dipInProgress) {
                // Start a new dip
                dipInProgress = true;
                dipCount++;
            }
        } else if (historySamples[i] >= dipRecoveryThreshold) {
            // Reset dip status when light level recovers
            dipInProgress = false;
        }
    }

    return dipCount;
}

static void *sampleLight(void *args){
    (void)args;
    while(!shutdown){
        
        pthread_mutex_lock(&data_mutex);
        
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

        double light = getRealWorldVoltage1();
        currentBuffer[currentSize] = light;
        currentSize++;
        samplesTaken++;
        
        if(currentSize == 0)
            averageVoltage = light;
        else 
            averageVoltage = light * DATA_WEIGHT + averageVoltage * (1 - DATA_WEIGHT);

        pthread_mutex_unlock(&data_mutex);

        sleepForMs(1);
    }
    return NULL;
}

void createSamplerThread(){
    pthread_create(&sampleThread, NULL, &sampleLight, NULL);
}

void joinSamplerThread(){
    pthread_join(sampleThread, NULL);
}

void shutdownSampler(){
    shutdown = true;
}