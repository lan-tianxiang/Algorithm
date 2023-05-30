#include "reverb.h"
#include "compressor.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    // Define the parameters of the reverb effect
    double delayTime = 0.8;
    const int delaySamples = sampleRate * delayTime;
    const double feedback = 0.5;
    const double wetLevel = 0.2;
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

    // Apply compression to the output signal
    //compressor(leftSignal, onceprocessSamples, 1.0, 1.5);
    //compressor(rightSignal, onceprocessSamples, 1.0, 1.5);

    // Free the memory allocated for the reflection line
    free(reflectionLine);
}
