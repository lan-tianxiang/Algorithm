/*
截止频率：2 kHz
窗函数：汉宁窗
采样率：48 kHz

滤波器长度的选择需要平衡频率响应的精度和计算复杂度。一种常见的选择是根据滤波器的主瓣宽度来确定滤波器长度。主瓣宽度取决于窗函数的特性。

对于汉宁窗，主瓣宽度约为4个主瓣宽度等于滤波器长度的1/4。因此，你可以尝试选择滤波器长度为4个主瓣宽度对应的采样点数。

主瓣宽度：
Mainlobe_width = 2 * π * Cutoff_frequency / Sampling_rate
Mainlobe_width = 2 * π * 2000 / 48000
Mainlobe_width ≈ 0.2618

滤波器长度：
Filter_length = 4 * Mainlobe_width * Sampling_rate / (2 * π)
Filter_length = 4 * 0.2618 * 48000 / (2 * π)
Filter_length ≈ 2102

根据计算，滤波器的长度约为2102个采样点。

接下来，根据滤波器长度和截止频率，可以使用窗函数法生成FIR滤波器的系数。使用汉宁窗函数，你可以按照以下步骤生成滤波器系数：

初始化一个长度为Filter_length的数组coefficients[]。
遍历数组的每个索引k，计算系数值：
coefficients[k] = 0.5 - 0.5 * cos(2 * π * k / (Filter_length - 1)), 其中 k 取值范围为 0 到 Filter_length - 1。
将滤波器系数归一化，使得它们的总和等于1：
total = 0
for each coefficient in coefficients:
total += coefficient
for each coefficient in coefficients:
coefficient /= total
生成的coefficients[]数组中存储了滤波器的系数，即汉宁窗低通滤波器的零点响应。
*/

#include <stdio.h>
#include <math.h>

#define FILTER_LENGTH 2102

// 定义滤波器系数数组
double coefficients[FILTER_LENGTH];

// 低通滤波函数
void lowPassFilter(double input[], double output[], int length)
{
    int i, j;
    
    for (i = 0; i < length; i++)
    {
        output[i] = 0.0;
        
        for (j = 0; j < FILTER_LENGTH; j++)
        {
            if (i - j >= 0)
            {
                output[i] += input[i - j] * coefficients[j];
            }
        }
    }
}

int main()
{
    // 计算滤波器系数
    int k;
    double total = 0.0;
    
    for (k = 0; k < FILTER_LENGTH; k++)
    {
        coefficients[k] = 0.5 - 0.5 * cos(2 * M_PI * k / (FILTER_LENGTH - 1));
        total += coefficients[k];
    }
    
    // 归一化滤波器系数
    for (k = 0; k < FILTER_LENGTH; k++)
    {
        coefficients[k] /= total;
    }
    
    // 输入信号和输出信号数组
    double input[100];
    double output[100];
    
    // 生成模拟的输入信号（3 kHz正弦信号）
    double frequency = 1000.0;  // 输入信号的频率
    double samplingRate = 48000.0;  // 采样率
    double amplitude = 1.0;  // 输入信号的幅值
    
    int numSamples = 100; // 输入/输出信号的长度
    double timeStep = 1.0 / samplingRate;
    int i;
    
    for (i = 0; i < numSamples; i++)
    {
        double t = i * timeStep;
        input[i] = amplitude * sin(2 * M_PI * frequency * t);
    }
    
    // 调用低通滤波函数
    lowPassFilter(input, output, numSamples);
    
    // 输出输入和输出信号的结果
    printf("输入信号:\n");
    for (i = 0; i < numSamples; i++)
    {
        printf("%f\n", input[i]);
    }
    
    printf("\n输出信号:\n");
    for (i = 0; i < numSamples; i++)
    {
        printf("%f\n", output[i]);
    }
    
    return 0;
}
