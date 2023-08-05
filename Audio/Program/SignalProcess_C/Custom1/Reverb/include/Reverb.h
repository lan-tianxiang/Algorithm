#ifndef REVERB_H
#define REVERB_H

#ifdef __cplusplus
extern "C" {
/*******************************************************************************************
 * @brief 该函数会对信号进行混响处理,主要原理是对信号进行多次反射,反射次数和反射时间间隔可以自行设置
 * 
 * @param inputSignal 输入信号
 * @param inputSignalSize 输入信号的长度
 * @param roomWidth 房间的宽度
 * @param roomHeight 房间的高度
 * @param roomDepth 房间的深度
 * @param reflectionCoefficient 反射系数
 * @param numReflectionLines 反射线的数量
 * @return float* 返回混响后的信号
*******************************************************************************************/
float* physicalModelingReverbAlgorithm(const float* inputSignal, int inputSignalSize, float roomWidth, float roomHeight, float roomDepth, float reflectionCoefficient, int numReflectionLines);
}
#endif//__cplusplus

#endif//REVERB_H
