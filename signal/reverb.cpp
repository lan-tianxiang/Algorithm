#include "reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    // Define the parameters of the reverb effect
    double delayTime = 0.1;
    const int delaySamples = sampleRate * delayTime;
    const double feedback = 0.3;
    const double wetLevel = 0.1;
    const double dryLevel = 1.0 - wetLevel;

    // Allocate memory for the reflection line
    double* reflectionLine = (double*)malloc(sizeof(double) * delaySamples);
    for (int i = 0; i < delaySamples; i++) {
        reflectionLine[i] = 0.0;
    }

    // Process each sample in the input signal
    for (int i = 0; i < onceprocessSamples; i++) {
        // Compute the reflected sample
        double reflectedSample = reflectionLine[i % delaySamples];

        // Compute the output sample
        double outputSample = leftSignal[i] * dryLevel + (reflectedSample + leftSignal[i]) * wetLevel;

        // Update the reflection line
        reflectionLine[i % delaySamples] = leftSignal[i] + feedback * reflectedSample;

        // Write the output sample to the left and right channels
        leftSignal[i] = outputSample;
        rightSignal[i] = outputSample;
    }

    // Free the memory allocated for the reflection line
    free(reflectionLine);
}
