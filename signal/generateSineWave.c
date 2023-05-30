#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sndfile.h>

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
void generateSineWave(double *signal, int onceprocessSamples, int *processedSamples, float risingTime, float stayTime, float cycleTime, float sampleRate, float minimumFreq, float maximumFreq) {
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
        signal[i] = (double)(amplitude * sin(currentPhase));
        *processedSamples %= cycleSamples;
    }
}

void audioPlayer(void *signal, int onceprocessSamples, float sampleRate) {
    float cycleTime = 15.0;//每个周期的时间
    float risingTime = cycleTime * 2.5 / 12.0;//上升时间
    float stayTime = cycleTime * 3.5 / 12.0;//保持时间
    float minimumFreq = 110;// 最低频率
    float maximumFreq = 870;// 最高频率
    
    static int processedSamples = 0;// 已经处理的样本数

    // 初始化
    memset(signal, 0, onceprocessSamples * sizeof(int));

    // 生成信号
    double* inputSignal = (double*)signal;
    generateSineWave(inputSignal, onceprocessSamples, &processedSamples, risingTime, stayTime, cycleTime, sampleRate, minimumFreq, maximumFreq);
}

int main() {
    float sampleRate = 48000;
    int signalTime = 15;
    int testSampleNum = sampleRate * signalTime;

    int onceprocessTime = 1;
    int onceprocessSamples = sampleRate * onceprocessTime; 
    int totalprocesscycle = testSampleNum / onceprocessSamples;
    double inputSignal[testSampleNum];

    for (int i = 0; i < (totalprocesscycle); i++) {
        audioPlayer(inputSignal + (i * onceprocessSamples), onceprocessSamples, sampleRate);
    }

    // Define the format of the output file
    SF_INFO sfinfo;
    sfinfo.samplerate = 48000;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;

    // Open the output file
    SNDFILE* wavfile = sf_open("output.wav", SFM_WRITE, &sfinfo);
    if (!wavfile) {
        printf("Error opening output file\n");
        return 1;
    }

    // Write the samples to the output file
    sf_count_t count = sf_write_double(wavfile, inputSignal, testSampleNum);
    if (count != testSampleNum) {
        printf("Error writing samples to output file\n");
        return 1;
    }

    // Close the output file
    sf_close(wavfile);

    // Open the output file
    FILE* txtfile = fopen("output.txt", "w");
    if (!txtfile) {
        printf("Error opening output file\n");
        return 1;
    }

    // Write the samples to the output file in text format
    for (int i = 0; i < testSampleNum; i++) {
        fprintf(txtfile, "%.14f\n", inputSignal[i]);
    }

    // Close the output file
    fclose(txtfile);

    return 0;
}