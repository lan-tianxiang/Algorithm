#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数会对信号进行混响处理
 * 
 * @param signal 信号
 * @param sampleNum 样本数
 * @param threshold 阈值
 * @param ratio 比率
*******************************************************************************************/
void compressor(double *signal, int sampleNum, double threshold, double ratio);
}
#endif//__cplusplus

#endif//COMPRESSOR_H
