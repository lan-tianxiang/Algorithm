#include "Reverb.h"
#include <stdlib.h> // Add this line to include stdlib.h
#include <string.h>
#include <math.h> // Add this line to include math.h
#include <vector>
#include <iostream> // Add this line to include iostream
/*
1.区分混响实现类型
【1】物理建模混响
【2】数字滤波器方法混响
    1.FIR混响
    2.IIR混响
    3.Convolution 混响
    4.Waveguide 混响
    5.Spectral 混响
*/

/*
在物理建模混响算法中，有几种选择反射线数量和位置的方法。下面是几种常见的：
1. 几何方法： 这些方法使用几何图形来确定反射线的位置。例如，一种方法是把反射线放在声波从长方形房间的墙壁上反射出来的地方。
2. 统计学方法： 这些方法使用统计分析来确定反射线的位置。例如，一种方法是使用蒙特卡洛模拟来随机生成反射线，然后选择产生最佳混响效果的反射线。
3. 优化方法： 这些方法使用优化算法来确定反射线的位置。例如，一种方法是使用遗传算法来演化出一套能产生最佳混响效果的反射线。
4. 经验方法： 这些方法使用试验和错误来确定反射线的位置。例如，一种方法是手动调整反射线的位置，直到达到理想的混响效果。
*/

/*
物理建模混响：
优点： 可以产生真实和自然的混响效果，类似于在真实世界的声学空间中的效果。
缺点：计算成本高，难以实现。
基于数字滤波器的混响：
优点： 可以有效地实现，可以产生广泛的混响效果。
缺点：听起来可能不如物理模型混响自然，而且容易出现假象，如振铃和色差。
卷积混响：
优点： 通过使用从真实世界的声学空间捕获的脉冲响应，可以产生高度真实的混响效果。
缺点：计算成本高，可能需要大量的内存来存储脉冲响应。
板式混响：
优点： 可以产生一种独特的、理想的混响声音，经常用于音乐制作中。
缺点：可能难以实现，可能需要专门的硬件。
弹簧混响：
优点： 可以产生一种独特的、理想的混响声音，经常用于吉他放大器。
缺点：可能难以实现，而且可能需要专门的硬件。
算法混响：
优点： 可以高度灵活，可以产生广泛的混响效果。
缺点：听起来可能不如物理模型混响自然，而且容易出现金属响声等伪影。
总的来说，混响算法的选择取决于具体的应用和期望的声音。有些应用可能需要高度逼真的混响效果，而其他应用可能优先考虑灵活性和易于实现。
*/

float calculateDistanceToLine(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
    // 计算点到直线的距离
    float a = y2 * z3 - y3 * z2 - y1 * z3 + y3 * z1 + y1 * z2 - y2 * z1;
    float b = x3 * z2 - x2 * z3 + x1 * z3 - x3 * z1 - x1 * z2 + x2 * z1;
    float c = x2 * y3 - x3 * y2 - x1 * y3 + x3 * y1 + x1 * y2 - x2 * y1;
    float d = -(a * x1 + b * y1 + c * z1);
    return fabsf(a * x2 + b * y2 + c * z2 + d) / sqrtf(a * a + b * b + c * c);
}

float calculateDistanceToWall(float x, float y, float z, float roomWidth, float roomHeight, float roomDepth) {
    // 计算点到墙壁的距离
    float distanceToLeftWall = x;
    float distanceToRightWall = roomWidth - x;
    float distanceToFrontWall = z;
    float distanceToBackWall = roomDepth - z;
    float distanceToCeiling = roomHeight - y;
    float distanceToFloor = y;
    return fminf(fminf(distanceToLeftWall, distanceToRightWall), fminf(fminf(distanceToFrontWall, distanceToBackWall), fminf(distanceToCeiling, distanceToFloor)));
}

int calculateNumReflections(float distanceToLine, float distanceToWall, float reflectionCoefficient) {
    // 计算声音在反射线和墙壁之间来回反射的次数
    float totalDistance = distanceToLine + distanceToWall * 2.0f;
    float totalReflections = logf(1.0f / reflectionCoefficient) / logf(1.0f - 1.0f / (totalDistance / 343.0f));
    return (int)totalReflections;
}

float* physicalModelingReverbAlgorithm(const float* inputSignal, int inputSignalSize, float roomWidth, float roomHeight, float roomDepth, float reflectionCoefficient, int numReflectionLines) {
    // 生成延迟线的位置
    float* reflectionLinePositions = (float*)malloc(numReflectionLines * 2 * sizeof(float));
    for (int i = 0; i < numReflectionLines; i++) {
        // 在房间的左侧和右侧各生成一条延迟线
        if (i % 2 == 0) {
            reflectionLinePositions[i * 2] = (i + 2) * roomWidth / (numReflectionLines + 1);
        } else {
            reflectionLinePositions[i * 2] = (i + 1) * roomWidth / (numReflectionLines + 1);
        }
        // 在房间的前侧和后侧各生成一条延迟线
        if (i % 2 == 0) {
            reflectionLinePositions[i * 2 + 1] = (i + 2) * roomDepth / (numReflectionLines + 1);
        } else {
            reflectionLinePositions[i * 2 + 1] = (i + 1) * roomDepth / (numReflectionLines + 1);
        }
    }

    // 计算每条延迟线的反射次数和衰减量
    float* outputSignal = (float*)malloc(inputSignalSize * sizeof(float));
    for (int i = 0; i < inputSignalSize; i++) {
        outputSignal[i] = 0.0f;
    }
    for (int i = 0; i < numReflectionLines * 2; i += 2) {
        float totalDelay = 0.0f;
        float totalAmplitude = 1.0f;
        for (int j = 0; j < inputSignalSize; j++) {
            // 计算声音从源点到反射线的距离
            float distanceToLine = calculateDistanceToLine(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, reflectionLinePositions[i], 0.0f, reflectionLinePositions[i + 1]);
            // 计算声音从反射线到墙壁的距离
            float distanceToWall = calculateDistanceToWall(reflectionLinePositions[i], 0.0f, reflectionLinePositions[i + 1], roomWidth, roomHeight, roomDepth);
            // 计算声音在反射线和墙壁之间来回反射的次数
            int numReflections = calculateNumReflections(distanceToLine, distanceToWall, reflectionCoefficient);
            // 计算声音在延迟线上的延迟量和衰减量
            float delay = distanceToLine + distanceToWall * 2.0f;
            float amplitude = powf(reflectionCoefficient, numReflections) / delay;
            // 将延迟量和衰减量累加到总量中
            totalDelay += delay;
            totalAmplitude *= amplitude;
            // 将延迟后的信号加入输出信号中
            if (j >= delay) {
                outputSignal[j] += inputSignal[j - (int)delay] * amplitude;
            }
        }
        // 输出每条延迟线的延迟量和衰减量
        printf("Reflection line %d delay: %f, amplitude: %f\n", i / 2, totalDelay, totalAmplitude);
    }

    free(reflectionLinePositions);
    return outputSignal;
}