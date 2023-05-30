#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#include "generateSineWave.h"
#include "reverb.h"
#include "stereo_surround.h"

#include <sndfile.h>

double* readWavFile(const char* filename, int* sampleNum, int* sampleRate, int* channels){
    // Open the input WAV file
    SF_INFO sfinfo;
    SNDFILE* wavfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!wavfile) {
        printf("Error opening input WAV file\n");
        return NULL;
    }

    // Read the samples from the input WAV file
    double* signal = (double*)malloc(sizeof(double) * sfinfo.frames);
    sf_count_t count = sf_read_double(wavfile, signal, sfinfo.frames);
    if (count != sfinfo.frames) {
        printf("Error reading samples from input WAV file\n");
        free(signal);
        return NULL;
    }

    // Close the input WAV file
    sf_close(wavfile);

    // Set the output parameters
    *sampleNum = sfinfo.frames;
    *sampleRate = sfinfo.samplerate;
    *channels = sfinfo.channels;

    return signal;
}

void writeWavFile(const char* wavFilename, double* signal, int sampleNum, int sampleRate) {
    // Define the format of the output file
    SF_INFO sfinfo;
    sfinfo.samplerate = sampleRate;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;

    // Open the output WAV file
    SNDFILE* wavfile = sf_open(wavFilename, SFM_WRITE, &sfinfo);
    if (!wavfile) {
        printf("Error opening output WAV file\n");
        return;
    }

    // Write the samples to the output WAV file
    sf_count_t count = sf_write_double(wavfile, signal, sampleNum);
    if (count != sampleNum) {
        printf("Error writing samples to output WAV file\n");
        return;
    }

    // Close the output WAV file
    sf_close(wavfile);
}

void writeTxtFile(const char* filename, double* signal, int sampleNum) {
    // Open the output TXT file
    FILE* txtfile = fopen(filename, "w");
    if (!txtfile) {
        printf("Error opening output TXT file\n");
        return;
    }

    // Write the samples to the output TXT file in text format
    for (int i = 0; i < sampleNum; i++) {
        fprintf(txtfile, "%.14f\n", signal[i]);
    }

    // Close the output TXT file
    fclose(txtfile);
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


    // 为了方便处理，将信号转换为双声道
    //double *leftSignal = (double *)malloc(sizeof(double) * onceprocessSamples);
    double *rightSignal = (double *)malloc(sizeof(double) * onceprocessSamples);
    //memset(leftSignal, 0, sizeof(double) * onceprocessSamples);
    memset(rightSignal, 0, sizeof(double) * onceprocessSamples);
    for (int i = 0; i < onceprocessSamples; i++) {
        //leftSignal[i] = inputSignal[i * 2];
        rightSignal[i] = inputSignal[i];
    }


    //surroundEffect(inputSignal, onceprocessSamples, sampleRate);
    reverbEffect(inputSignal, rightSignal, onceprocessSamples, sampleRate);

    free(rightSignal);

    // 拷贝信号
    //memcpy(inputSignal, leftSignal, sizeof(float) * onceprocessSamples);
}

int main() {
    /*
    float sampleRate = 48000;
    int signalTime = 15;
    int testSampleNum = sampleRate * signalTime;

    int onceprocessTime = 1;
    int onceprocessSamples = (int)(sampleRate * onceprocessTime); 
    int totalprocesscycle = testSampleNum / onceprocessSamples;
    double inputSignal[testSampleNum];
    for (int i = 0; i < (totalprocesscycle); i++) {
        audioPlayer(inputSignal + (i * onceprocessSamples), onceprocessSamples, sampleRate);
    }
    */
    int testSampleNum;
    int sampleRate;
    int channels;
    double* inputSignal = readWavFile("input.wav", &testSampleNum, &sampleRate, &channels);
    if (!inputSignal) {
        printf("Error reading input WAV file\n");
        return 1;
    }

    reverbEffect(inputSignal, inputSignal, testSampleNum, sampleRate);

    writeWavFile("output.wav", inputSignal, testSampleNum, sampleRate);
    /*
    writeTxtFile("output.txt", inputSignal, testSampleNum);
    */

    free(inputSignal);
    return 0;
}
