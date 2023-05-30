#include "compressor.h"
#include <math.h>

void compressor(double *signal, int sampleNum, double threshold, double ratio) {
    // Compute the gain reduction factor
    double gainReduction = 0.0;
    if (threshold > 0.0) {
        gainReduction = (1.0 - 1.0 / ratio) / threshold;
    }

    // Apply the gain reduction to the signal
    for (int i = 0; i < sampleNum; i++) {
        double absValue = fabs(signal[i]);
        if (absValue > threshold) {
            double gain = threshold + (absValue - threshold) * gainReduction;
            signal[i] *= gain / absValue;
        }
    }
}