#include "reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect_reflectionLines(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    // Define the parameters of the reverb effect
    double delayTime = 0.005;
    int delaySamples = sampleRate * delayTime;
    double feedback = 0.3;
    double wetLevel = 0.1;
    double dryLevel = 1.0 - wetLevel;

    // Define the number of reflection lines
    int numLines = 1;

    // Allocate memory for the reflection line
    double** reflectionLines = (double**)malloc(sizeof(double*) * numLines);
    for (int i = 0; i < numLines; i++) {
        reflectionLines[i] = (double*)malloc(sizeof(double) * delaySamples);
        for (int j = 0; j < delaySamples; j++) {
            reflectionLines[i][j] = 0.0;
        }
    }

    // Process each sample in the input signal
    for (int i = 0; i < onceprocessSamples; i++) {
        // Compute the output of each reflection line
        double outputSamples[numLines];
        for (int j = 0; j < numLines; j++) {
            double reflectedSample = reflectionLines[j][i % delaySamples];
            outputSamples[j] = reflectedSample;
        }

        // Compute the weighted sum of the reflection line outputs
        double outputSample = 0.0;
        for (int j = 0; j < numLines; j++) {
            outputSample += outputSamples[j] * wetLevel / numLines;
        }
        outputSample += leftSignal[i] * dryLevel;

        // Update the reflection lines
        for (int j = 0; j < numLines; j++) {
            reflectionLines[j][i % delaySamples] = leftSignal[i] + feedback * outputSamples[j];
        }

        // Write the output sample to the left and right channels
        leftSignal[i] = outputSample;
        rightSignal[i] = outputSample;
    }

    // Free the memory allocated for the reflection line
    free(reflectionLines);
}
