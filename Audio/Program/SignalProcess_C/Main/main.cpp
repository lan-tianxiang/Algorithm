#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#include "Reverb.h"
#include "StereoSurround.h"
#include "Compressor.h"
#include "SineWaveGenerator.h"
#include "Freeverb.hpp"

float* readWavFile(const char* filename, int* sampleNum, int* sampleRate, int* channels){
    // Open the input WAV file
    SF_INFO sfinfo;
    SNDFILE* wavfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!wavfile) {
        printf("Error opening input WAV file\n");
        return NULL;
    }

    // Read the samples from the input WAV file
    float* signal = (float*)malloc(sizeof(float) * sfinfo.frames);
    sf_count_t count = sf_read_float(wavfile, signal, sfinfo.frames);
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

void writeWavFile(const char* wavFilename, float* signal, int sampleNum, int sampleRate, int channels) {
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
    sf_count_t count = sf_write_float(wavfile, signal, sampleNum);
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

void audioPlayer(void *signal, int onceprocessSamples, float sampleRate, float cycleTime) {
    float risingTime = cycleTime * 2.5 / 12.0;//上升时间
    float stayTime = cycleTime * 3.5 / 12.0;//保持时间
    float minimumFreq = 110;// 最低频率
    float maximumFreq = 870;// 最高频率
    
    static int processedSamples = 0;// 已经处理的样本数

    // 初始化
    memset(signal, 0, onceprocessSamples * sizeof(int));

    // 生成信号
    float* inputSignal = (float*)signal;
    generateSineWave(inputSignal, onceprocessSamples, &processedSamples, risingTime, stayTime, cycleTime, sampleRate, minimumFreq, maximumFreq);
}


int signal_tester(void) {
    const float sampleRate = 48000;
    const int signalTime = 20;
    const int testSampleNum = sampleRate * signalTime;

    static float inputSignal_L[testSampleNum];
    static float inputSignal_R[testSampleNum];
    static float inputSignal[testSampleNum * 2];

    static float outputSignal_L[testSampleNum];
    static float outputSignal_R[testSampleNum];
    static float outputSignal[testSampleNum * 2];

    audioPlayer(inputSignal_R, testSampleNum, sampleRate, (float)signalTime/2.0);
    for (int i = 0; i < testSampleNum; i++) {
        inputSignal_L[i] = inputSignal_R[i];
    }

    // Apply 6dB gain to the output signal to compensate for the attenuation of the reverb effect
    // avoid Clipping distortion caused by the reverb effect in the same time
    for (int i = 0; i < testSampleNum; i++) {
        inputSignal_L[i] *= 0.5;
        inputSignal_R[i] *= 0.5;
    }

    for (int i = 0; i < testSampleNum; i++) {
        outputSignal_L[i] = inputSignal_L[i];
        outputSignal_R[i] = inputSignal_R[i];
    }
    
    //memset(outputSignal_L, 0, testSampleNum * sizeof(double));
    //memset(outputSignal_R, 0, testSampleNum * sizeof(double));    
    //outputSignal_L[5000] = 1;

    // Apply reverb to the output signal
    //physicalModelingReverbAlgorithm(outputSignal_L, testSampleNum, 5, 0.5, 0.5, 0.5);
    
    // Apply compression to the output signal
    //compressor(outputSignal_L, testSampleNum, 0.9, 2.0);
    //compressor(outputSignal_R, testSampleNum, 0.9, 2.0);
    
    // Apply stereo surround to the output signal
    //surroundEffect(outputSignal_L, outputSignal_R, testSampleNum, sampleRate);
    Freeverb* freeverb = new Freeverb();
    freeverb->setParameter(KMode, 0.5);
    freeverb->setParameter(KRoomSize, 0.5);
    freeverb->setParameter(KDamp, 0.5);
    freeverb->setParameter(KWet, 0.5);
    freeverb->setParameter(KDry, 0.5);
    freeverb->setParameter(KWidth, 0.5);

    float** freeverbInputSignal = new float*[2];
    float** freeverbOutputSignal = new float*[2];
    freeverbInputSignal[0] = inputSignal_L;
    freeverbInputSignal[1] = inputSignal_R;
    freeverbOutputSignal[0] = outputSignal_L;
    freeverbOutputSignal[1] = outputSignal_R;

    freeverb->process(freeverbInputSignal, freeverbOutputSignal, testSampleNum);

    for (int i = 0; i < testSampleNum; i++) {
        inputSignal[i * 2] = inputSignal_L[i];
        inputSignal[i * 2 + 1] = inputSignal_R[i];
    }

    for (int i = 0; i < testSampleNum; i++) {
        outputSignal[i * 2] = outputSignal_L[i];
        outputSignal[i * 2 + 1] = outputSignal_R[i];
    }

    //writeTxtFile("output.txt", inputSignal + testSampleNum, testSampleNum);
    writeWavFile("inputSignal.wav", inputSignal, testSampleNum, sampleRate, 2);
    writeWavFile("outputSignal.wav", outputSignal, testSampleNum, sampleRate, 2);

    return 0;
}


int signal_tester_2(const char* inputfilename_L, const char* inputfilename_R, const char* outputfilename) {
    int testSampleNum;
    int sampleRate;
    int channels;
    float* inputSignal_L = readWavFile(inputfilename_L, &testSampleNum, &sampleRate, &channels);
    float* inputSignal_R = readWavFile(inputfilename_R, &testSampleNum, &sampleRate, &channels);

    static float outputSignal[48000*240 * 2];
    
    if (!inputSignal_L || !inputSignal_R) {
        printf("Error reading input WAV file\n");
        return 1;
    }
    
    // Apply 6dB gain to the output signal to compensate for the attenuation of the reverb effect
    // avoid Clipping distortion caused by the reverb effect in the same time
    for (int i = 0; i < testSampleNum; i++) {
        inputSignal_L[i] *= 0.5;
        inputSignal_R[i] *= 0.5;
    }

    memset(inputSignal_L, 0, testSampleNum * sizeof(float));
    memset(inputSignal_R, 0, testSampleNum * sizeof(float));
    inputSignal_L[5000] = 0.9f;
    memset(outputSignal, 0, 48000*240 * 2 * sizeof(float));
    
    // Apply reverb to the output signal
    float* inputSignal_123 = NULL;
    inputSignal_123 = physicalModelingReverbAlgorithm(inputSignal_L, testSampleNum, 5, 0.5, 0.5, 0.5, 0.5);
    
    // Apply stereo surround to the output signal
    //surroundEffect(inputSignal_L, inputSignal_R, testSampleNum, sampleRate);
    
    // Apply compression to the output signal
    //compressor(inputSignal_L, testSampleNum, 0.9, 2.0);
    //compressor(inputSignal_R, testSampleNum, 0.9, 2.0);
    
    //让inputSignal组成立体声格式
    for (int i = 0; i < testSampleNum; i++) {
        outputSignal[i * 2] = inputSignal_123[i] * 2;
        outputSignal[i * 2 + 1] = inputSignal_R[i] * 2;
    }
    
    channels = 2;
    writeWavFile(outputfilename, outputSignal, testSampleNum*2, sampleRate, channels);

    free(inputSignal_L);
    free(inputSignal_R);
    return 0;
}

int main() {
    signal_tester();
    //signal_tester_2("input_L.wav", "input_R.wav", "output.wav");
    return 0;
}
