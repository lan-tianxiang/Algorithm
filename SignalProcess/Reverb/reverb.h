#ifndef REVERB_H
#define REVERB_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数会对信号进行混响处理,主要原理是对信号进行多次反射,反射次数和反射时间间隔可以自行设置
 * 
 * @param leftSignal 左声道信号
 * @param rightSignal 右声道信号
 * @param onceprocessSamples 单次处理样本数
 * @param sampleRate 采样率
*******************************************************************************************/
void physicalModelingReverbAlgorithm(double* leftSignal, double* rightSignal, int onceprocessSamples, float sampleRate);
}
#endif//__cplusplus

#endif//REVERB_H
