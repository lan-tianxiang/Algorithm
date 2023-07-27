#include "Compressor.h"
#include <math.h>

void compressor(double *signal, int sampleNum, double threshold, double ratio) {
    // Compute the gain reduction factor
    double gainReduction = 0.0;
    if (threshold > 0.0) {
        gainReduction = (1.0 - (1.0 / ratio)) / threshold;
    }

    // Apply the gain reduction to the signal
    for (int i = 0; i < sampleNum; i++) {
        double absValue = fabs(signal[i]);
        if (absValue > threshold) {
            double gain = threshold + ((absValue - threshold) * gainReduction);
            signal[i] *= (gain / absValue);
        }
    }
}

/****************************************************************************************
 * 伪代码
 * 1. 计算增益缩减因子
 * 2. 将增益缩减应用到信号上
 * 3. 伪代码
 ****************************************************************************************/
/*
    if absValue of signal[i] > threshold, then gain = threshold + (absValue - threshold) * gainReduction
    for example:
        signal[i] = -1.3
        absValue = 1.3
        threshold = 0.9
        ratio = 3.0

        gainReduction = (1.0 - (1.0 / ratio)) / threshold = (1.0 - (1.0 / 3.0)) / 0.9 = (1.0 - 0.3333333333333333) / 0.9 = 0.6666666666666666 / 0.9 = 0.7407407407407407
        gain = 0.9 + (1.3 - 0.9) * 0.7407407407407407 = 0.9 + 0.4 * 0.7407407407407407 = 0.9 + 0.2962962962962963 = 1.1962962962962963
        signal[i] = signal[i] * (gain / absValue) = signal[i] * (1.1962962962962963 / 1.3) = signal[i] * 0.9202296137339056 = -1.3 * 0.9202296137339056 = -1.1962962962962963
        so, in this case, the signal[i] is compressed from -1.3 to -1.1962962962962963
        but it's still larger than the threshold, so it will be compressed again in the next loop
*/

