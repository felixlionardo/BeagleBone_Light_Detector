#ifndef _SAMPLER_H_
#define _SAMPLER_H_

/* Module to initialize and cleanup the sampler thread.
The sampler thread handles sampling data from the A2D light sensor to store and update history data.
It shares a lock data_mutex with print_result.c for thread-safe access to the history data.
*/

#include <stdbool.h>
#include <pthread.h>


void setCurrentSize(int);
void setHistorySize(int);
void setHistoryDips(int);

pthread_mutex_t* get_mutex();

int getHistorySize();
int getCurrentSize();
int getHistoryDips();

long long getSamplesTaken();
double getAverageVoltage();
double *getHistoryBuffer();

void moveCurrentDataToHistory();
int analyzeLightDips();

void createSamplerThread();
void joinSamplerThread();
void shutdownSampler();

#endif
