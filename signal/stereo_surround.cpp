#include "stereo_surround.h"
#include <stdlib.h>
#include <string.h>

void surroundEffect(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    float delayTime = 0.008f; // 30ms 的延迟时间
    int delaySamples = (int)(delayTime * sampleRate);
    float feedback = 0.5f; // 延迟反馈系数
    float *delayLine_L = (float *)malloc(delaySamples * sizeof(float));
    float *delayLine_R = (float *)malloc(delaySamples * sizeof(float));

    memset(delayLine_L, 0, delaySamples * sizeof(float));
    memset(delayLine_R, 0, delaySamples * sizeof(float));

    for (int i = 0; i < onceprocessSamples; i++) {
        float delayedSample_L = delayLine_L[i % delaySamples];
        float delayedSample_R = delayLine_R[i % delaySamples];

        leftSignal[i] += delayedSample_L;
        rightSignal[i] += delayedSample_R;

        delayLine_L[i % delaySamples] = rightSignal[i] * feedback;
        delayLine_R[i % delaySamples] = leftSignal[i] * feedback;
    }

    // Free the memory allocated for the delay lines
    free(delayLine_L);
    free(delayLine_R);
}
