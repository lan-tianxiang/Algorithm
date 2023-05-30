#include "reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect(void *signal, int onceprocessSamples, float sampleRate) {
    float *input = (float *)signal;
    float *left = input;
    float *right = input + onceprocessSamples;

    float delayTime = 0.05f; // 50ms 的延迟时间
    int delaySamples = (int)(delayTime * sampleRate);
    float feedback = 0.5f; // 延迟反馈系数
    float *delayLine = (float *)malloc(delaySamples * sizeof(float));
    memset(delayLine, 0, delaySamples * sizeof(float));
    float *reflectionLine = (float *)malloc(delaySamples * sizeof(float));
    memset(reflectionLine, 0, delaySamples * sizeof(float));
    float delayGain = 0.5f; // 延迟增益
    float reflectionGain = 0.3f; // 反射增益
    for (int i = 0; i < onceprocessSamples; i++) {
        float delayedSample = delayLine[i % delaySamples];
        float reflectedSample = reflectionLine[i % delaySamples];
        left[i] = input[i] + delayGain * delayedSample + reflectionGain * reflectedSample;
        right[i] = input[i] + delayGain * delayedSample - reflectionGain * reflectedSample;
        delayLine[i % delaySamples] = input[i] + feedback * delayedSample;
        reflectionLine[i % delaySamples] = input[i] + feedback * reflectedSample;
    }
    free(delayLine);
    free(reflectionLine);
}
