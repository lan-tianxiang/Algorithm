#include "reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    double delayTime = 0.05f; // 50ms 的延迟时间
    int delaySamples = (int)(delayTime * sampleRate);
    double feedback = 0.5f; // 延迟反馈系数
    double *delayLine = (double *)malloc(delaySamples * sizeof(double));
    memset(delayLine, 0, delaySamples * sizeof(double));
    double *reflectionLine = (double *)malloc(delaySamples * sizeof(double));
    memset(reflectionLine, 0, delaySamples * sizeof(double));

    double delayGain = 0.5f; // 延迟增益
    double reflectionGain = 0.3f; // 反射增益

    for (int i = 0; i < onceprocessSamples; i++) {
        double delayedSample = delayLine[i % delaySamples];
        double reflectedSample = reflectionLine[i % delaySamples];

        leftSignal[i] = leftSignal[i] + delayGain * delayedSample + reflectionGain * reflectedSample;
        rightSignal[i] = rightSignal[i] + delayGain * delayedSample + reflectionGain * reflectedSample;

        //delayLine[i % delaySamples] = leftSignal[i] + feedback * delayedSample;
        //reflectionLine[i % delaySamples] = (leftSignal[i] + rightSignal[i]) / 2.0 + feedback * reflectedSample;
    }

    free(delayLine);
    free(reflectionLine);
}
