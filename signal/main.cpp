#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#include "generateSineWave.h"
#include "reverb.h"
#include "stereo_surround.h"
#include "compressor.h"

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

void writeWavFile(const char* wavFilename, double* signal, int sampleNum, int sampleRate, int channels) {
    // Define the format of the output file
    SF_INFO sfinfo;
    sfinfo.samplerate = sampleRate;
    sfinfo.channels = channels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;

    // Open the output WAV file
    SNDFILE* wavfile = sf_open(wavFilename, SFM_WRITE, &sfinfo);
    if (!wavfile) {
        printf("Error opening output WAV file\n");
        return;
    }

    // Write the samples to the output WAV file
    sf_count_t count = sf_write_double(wavfile, signal, sampleNum);
    if (count != (sf_count_t)sampleNum) {
        printf("Error writing samples to output WAV file %ld != %d\n", count, sampleNum);
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
}

int main() {
    
    float sampleRate = 48000;
    int signalTime = 5;
    int testSampleNum = sampleRate * signalTime;

    int onceprocessTime = 1;
    int onceprocessSamples = (int)(sampleRate * onceprocessTime); 
    int totalprocesscycle = testSampleNum / onceprocessSamples;
    double inputSignal[testSampleNum*2];
    double outputSignal[testSampleNum*2];
    for (int i = 0; i < (totalprocesscycle); i++) {
        audioPlayer(inputSignal + (i * onceprocessSamples), onceprocessSamples, sampleRate);
    }
    memcpy(inputSignal + testSampleNum, inputSignal, testSampleNum * sizeof(double));

    // Apply 6dB gain to the output signal
    for (int i = 0; i < testSampleNum; i++) {
        inputSignal[i] *= 0.5;
        inputSignal[i+testSampleNum] *= 0.5;
    }

    memset(inputSignal, 0, testSampleNum * sizeof(double));
    memset(inputSignal + testSampleNum, 0, testSampleNum * sizeof(double));
    inputSignal[5000] = 1;
    memcpy(inputSignal + testSampleNum, inputSignal, testSampleNum * sizeof(double));
    // Apply reverb to the output signal
    reverbEffect(inputSignal + testSampleNum, inputSignal + testSampleNum, testSampleNum, sampleRate);
    
    // Apply compression to the output signal
    //compressor(inputSignal + testSampleNum, onceprocessSamples, 0.9, 2.0);
    
    //让inputSignal组成立体声格式
    for (int i = 0; i < testSampleNum; i++) {
        outputSignal[i * 2] = inputSignal[i];
        outputSignal[i * 2 + 1] = inputSignal[i+testSampleNum];
    }

    writeTxtFile("output.txt", inputSignal + testSampleNum, testSampleNum);
    writeWavFile("output.wav", outputSignal, testSampleNum, sampleRate, 2);
    
    
    /*
    int testSampleNum;
    int sampleRate;
    int channels;
    double* inputSignal = readWavFile("input.wav", &testSampleNum, &sampleRate, &channels);
    double* inputSignal_2 = (double*)malloc(sizeof(double) * testSampleNum);
    double* outputSignal = (double*)malloc(sizeof(double) * testSampleNum * 4);
    
    if (!inputSignal) {
        printf("Error reading input WAV file\n");
        return 1;
    }
    
    // Apply 6dB gain to the output signal
    for (int i = 0; i < testSampleNum; i++) {
        inputSignal[i] *= 0.5;
    }

    memcpy(inputSignal_2, inputSignal, testSampleNum * sizeof(double));

    //surroundEffect(inputSignal, onceprocessSamples, sampleRate);
    reverbEffect(inputSignal, inputSignal, testSampleNum, sampleRate);

    // Apply compression to the output signal
    //compressor(inputSignal, testSampleNum, 0.9, 2.0);
    
    //让inputSignal组成立体声格式
    for (int i = 0; i < testSampleNum; i++) {
        outputSignal[i * 2] = inputSignal_2[i];
        outputSignal[i * 2 + 1] = inputSignal[i];
    }

    channels = 2;
    writeWavFile("output.wav", outputSignal, testSampleNum, sampleRate, channels);

    free(inputSignal);
    free(inputSignal_2);
    free(outputSignal);
    */
    return 0;
}
