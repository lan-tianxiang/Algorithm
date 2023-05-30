#ifndef REVERB_H
#define REVERB_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数会对信号进行混响处理
 * 
 * @param signal 信号
 * @param onceprocessSamples 单次处理样本数
 * @param sampleRate 采样率
*******************************************************************************************/
void reverbEffect(void *signal, int onceprocessSamples, float sampleRate);
}
#endif//__cplusplus

#endif//REVERB_H
