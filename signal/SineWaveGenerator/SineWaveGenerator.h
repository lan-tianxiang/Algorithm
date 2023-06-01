#ifndef SINWAVEGENERATOR_H
#define SINWAVEGENERATOR_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数用于生成正弦波
 * @param signal 生成的正弦波
 * @param onceprocessSamples 一次处理的样点数
 * @param processedSamples 已处理的样点数
 * @param risingTime 上升时间
 * @param stayTime 停留时间
 * @param cycleTime 周期时间
 * @param sampleRate 采样率
 * @param minimumFreq 最小频率
 * @param maximumFreq 最大频率
 * @return void
*******************************************************************************************/
void generateSineWave(double *signal, int onceprocessSamples, int *processedSamples, float risingTime, float stayTime, float cycleTime, float sampleRate, float minimumFreq, float maximumFreq);
}
#endif//__cplusplus

#endif//SINWAVEGENERATOR_H
