#ifndef STEREOSURROUND_H
#define STEREOSURROUND_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数会对信号进行环绕声处理
 * 
 * @param leftSignal 左声道信号
 * @param rightSignal 右声道信号
 * @param onceprocessSamples 单次处理样本数
 * @param sampleRate 采样率
*******************************************************************************************/
void surroundEffect(double *leftSignal, double *rightSignal, int onceprocessSamples, float sampleRate);
}
#endif//__cplusplus

#endif//STEREOSURROUND_H
