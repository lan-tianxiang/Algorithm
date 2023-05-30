#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#include "generateSineWave.h"
#include "reverb.h"
#include "stereo_surround.h"

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
    //surroundEffect(inputSignal, onceprocessSamples, sampleRate);
    //reverbEffect(inputSignal, onceprocessSamples, sampleRate);
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
