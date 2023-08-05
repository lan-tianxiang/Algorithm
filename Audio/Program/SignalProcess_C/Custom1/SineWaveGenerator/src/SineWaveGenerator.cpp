#include "SineWaveGenerator.h"
#include <math.h>

#define PI 3.14159265358979323846

float factorial(int n) {
    float result = 1.0f;
    int i;
    for (i = 2; i <= n; i++) {
        result *= (float)i;
    }
    return result;
}

float power(float x, int n) {
    float result = 1.0f;
    int i;
    for (i = 0; i < n; i++) {
        result *= x;
    }
    return result;
}

float sinf_approximate(float x) {
    // 用泰勒级数展开计算 sin(x)
    float result = 0.0f;
    float term = x;
    int sign = 1;
    int n = 1;
    float epsilon = 1e-6f;  // 根据 float 类型的精度选择合适的 epsilon 值

    while (term > epsilon || term < -epsilon) {
        result += term;
        sign *= -1;
        float factorial_odd = factorial(2 * n + 1);
        term = sign * (power(x, 2 * n + 1) / factorial_odd);
        n++;
    }

    return result;
}

//函数原型为void generateSineWave(int *signal, int onceprocessSamples, int processedSamples, float risingTime, float stayTime, float cycleTime, float sampleRate, float minimumFreq, float maximumFreq);
//其中onceprocessSamples为单次处理样本数，processedSamples为已处理样本数，risingTime为上升时间，stayTime为保持时间，cycleTime为一个周期的时间，sampleRate为采样率，minimumFreq为最低频率，maximumFreq为最高频率。
//该函数会生成一个完整周期的正弦波信号，正弦波的频率随着processedSamples数的增加而增加，processedSamples数到达risingSamples时达到最大频率，接着保持最大频率stayTime时间，
//接着正弦波的频率随着processedSamples数的增加而减小，processedSamples数到达cycleSamples时达到最小频率频率。
void generateSineWave(float *signal, int onceprocessSamples, int *processedSamples, float risingTime, float stayTime, float cycleTime, float sampleRate, float minimumFreq, float maximumFreq) {
    int risingSamples = (int)(risingTime * sampleRate);
    int staySamples = (int)(stayTime * sampleRate);
    int cycleSamples = (int)(cycleTime * sampleRate);
    float freqRange = maximumFreq - minimumFreq;
    float freqIncrement = freqRange / risingSamples;
    float freqDecrement = freqRange / (cycleSamples - risingSamples - staySamples);
    static float currentPhase = 0.0;
    float currentFreq;

    for (int i = 0; i < onceprocessSamples; i++, (*processedSamples)++) {
        if ((*processedSamples) < risingSamples) {
            // 从最小频率逐渐增加到最大频率
            currentFreq = minimumFreq + (freqIncrement * (*processedSamples));
        } else if ((*processedSamples) < (staySamples+ risingSamples)) {
            // 保持最大频率
            currentFreq = maximumFreq;
        } else {
            // 从最大频率逐渐减小到最小频率
            currentFreq = maximumFreq - (freqDecrement * (*processedSamples - risingSamples - staySamples));
        }

        double amplitude = 1.0f;
        currentPhase += 2 * PI * currentFreq / sampleRate;
        while (currentPhase > 2 * PI) {
            currentPhase -= 2 * PI;
        }
        while (currentPhase < -2 * PI) {
            currentPhase += 2 * PI;
        }
        signal[i] = (float)(amplitude * sinf(currentPhase));
        *processedSamples %= cycleSamples;
    }
}
