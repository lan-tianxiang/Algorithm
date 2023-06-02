#include "reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>

void reverbEffect_reflectionLines(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate) {
    // Define the parameters of the reverb effect
    double delayTime = 0.08;
    int delaySamples = sampleRate * delayTime;
    double feedback = 0.2;
    double wetLevel = 0.3;
    double dryLevel = 1.0 - wetLevel;

    // Define the number of reflection lines
    int numLines = 1;

    // Allocate memory for the reflection lines of the left channel
    double** leftReflectionLines = (double**)malloc(sizeof(double*) * numLines);
    for (int i = 0; i < numLines; i++) {
        leftReflectionLines[i] = (double*)malloc(sizeof(double) * delaySamples);
        for (int j = 0; j < delaySamples; j++) {
            leftReflectionLines[i][j] = 0.0;
        }
    }

    // Allocate memory for the reflection lines of the right channel
    double** rightReflectionLines = (double**)malloc(sizeof(double*) * numLines);
    for (int i = 0; i < numLines; i++) {
        rightReflectionLines[i] = (double*)malloc(sizeof(double) * delaySamples);
        for (int j = 0; j < delaySamples; j++) {
            rightReflectionLines[i][j] = 0.0;
        }
    }

    // Process each sample in the input signal
    for (int i = 0; i < onceprocessSamples; i++) {
        // Compute the output of each reflection line for the left channel
        double leftOutputSamples[numLines];
        for (int j = 0; j < numLines; j++) {
            leftOutputSamples[j] = leftReflectionLines[j][i % delaySamples];
        }

        // Update the reflection lines for the left channel
        for (int j = 0; j < numLines; j++) {
            // Compute the new delay line for the current reflection line
            double newDelayLine = leftSignal[i] + (feedback * leftOutputSamples[j]);

            // Store the new delay line in the reflection line buffer
            leftReflectionLines[j][i % delaySamples] = newDelayLine;

            // Update the output sample for the current reflection line
            leftOutputSamples[j] = leftReflectionLines[j][(i - delaySamples + 1 + delaySamples) % delaySamples];
        }

        // Compute the weighted sum of the reflection line outputs for the left channel
        double leftOutputSample = 0.0;
        for (int j = 0; j < numLines; j++) {
            leftOutputSample += leftOutputSamples[j] * wetLevel / numLines;
        }
        leftOutputSample += leftSignal[i] * dryLevel;

        // Compute the output of each reflection line for the right channel
        double rightOutputSamples[numLines];
        for (int j = 0; j < numLines; j++) {
            rightOutputSamples[j] = rightReflectionLines[j][i % delaySamples];
        }

        // Update the reflection lines for the right channel
        for (int j = 0; j < numLines; j++) {
            // Compute the new delay line for the current reflection line
            double newDelayLine = rightSignal[i] + (feedback * rightOutputSamples[j]);

            // Store the new delay line in the reflection line buffer
            rightReflectionLines[j][i % delaySamples] = newDelayLine;

            // Update the output sample for the current reflection line
            rightOutputSamples[j] = rightReflectionLines[j][(i - delaySamples + 1 + delaySamples) % delaySamples];
        }

        // Compute the weighted sum of the reflection line outputs for the left channel
        double rightOutputSample = 0.0;
        for (int j = 0; j < numLines; j++) {
            rightOutputSample += rightOutputSamples[j] * wetLevel / numLines;
        }
        rightOutputSample += rightSignal[i] * dryLevel;

        // Mix the left and right channel outputs
        leftSignal[i] = leftOutputSample;
        rightSignal[i] = rightOutputSample;
    }

    // Free the memory allocated for the reflection line
    for (int i = 0; i < numLines; i++) {
        free(leftReflectionLines[i]);
        free(rightReflectionLines[i]);
    }
    free(leftReflectionLines);
    free(rightReflectionLines);
}
