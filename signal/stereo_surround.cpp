#include "stereo_surround.h"
#include <stdlib.h>
#include <string.h>

void surroundEffect(void *signal, int onceprocessSamples, float sampleRate) {
    float *input = (float *)signal;
    float *left = input;
    float *right = input + onceprocessSamples;
    float delayTime = 0.03f; // 30ms 的延迟时间
    int delaySamples = (int)(delayTime * sampleRate);
    float feedback = 0.5f; // 延迟反馈系数
    float *delayLine = (float *)malloc(delaySamples * sizeof(float));
    memset(delayLine, 0, delaySamples * sizeof(float));
    for (int i = 0; i < onceprocessSamples; i++) {
        float delayedSample = delayLine[i % delaySamples];
        left[i] += delayedSample;
        right[i] -= delayedSample;
        delayLine[i % delaySamples] = input[i] + feedback * delayedSample;
    }
    free(delayLine);
}
