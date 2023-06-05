#include "reverb.h"
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

// 计算声音从源点到反射线的距离
float calculateDistanceToLine(float sourceX, float sourceY, float sourceZ, float sourceDirectionX, float sourceDirectionY, float sourceDirectionZ, float lineX, float lineY, float lineZ) {
    float dx = lineX - sourceX;
    float dy = lineY - sourceY;
    float dz = lineZ - sourceZ;
    float dot = dx * sourceDirectionX + dy * sourceDirectionY + dz * sourceDirectionZ;
    float distance = sqrtf(dx * dx + dy * dy + dz * dz - dot * dot);
    return distance;
}

// 计算声音从反射线到墙壁的距离
float calculateDistanceToWall(float lineX, float lineY, float lineZ, float roomWidth, float roomHeight, float roomDepth) {
    float distanceToWall = 0.0f;
    if (lineX == 0.0f || lineX == roomWidth) {
        distanceToWall = lineZ < roomDepth / 2.0f ? lineZ : roomDepth - lineZ;
    } else if (lineZ == 0.0f || lineZ == roomDepth) {
        distanceToWall = lineX < roomWidth / 2.0f ? lineX : roomWidth - lineX;
    } else {
        distanceToWall = lineY < roomHeight / 2.0f ? lineY : roomHeight - lineY;
    }
    return distanceToWall;
}

// 计算声音在反射线和墙壁之间来回反射的次数
int calculateNumReflections(float distanceToLine, float distanceToWall, float reflectionCoefficient) {
    float totalDistance = distanceToLine + distanceToWall;
    float totalReflections = logf(0.001f) / logf(reflectionCoefficient);
    float numReflections = totalReflections * totalDistance / distanceToLine;
    return (int)numReflections;
}

// physicalModelingReverbAlgorithm 函数的实现
std::vector<float> physicalModelingReverbAlgorithm(const std::vector<float>& inputSignal, float roomWidth, float roomHeight, float roomDepth, float reflectionCoefficient, int numReflectionLines) {
    // 生成延迟线的位置
    std::vector<float> reflectionLinePositions;
    for (int i = 0; i < numReflectionLines; i++) {
        // 在房间的左侧和右侧各生成一条延迟线
        if (i % 2 == 0) {
            reflectionLinePositions.push_back((i + 2) * roomWidth / (numReflectionLines + 1));
        } else {
            reflectionLinePositions.push_back((i + 1) * roomWidth / (numReflectionLines + 1));
        }
        // 在房间的前侧和后侧各生成一条延迟线
        if (i % 2 == 0) {
            reflectionLinePositions.push_back((i + 2) * roomDepth / (numReflectionLines + 1));
        } else {
            reflectionLinePositions.push_back((i + 1) * roomDepth / (numReflectionLines + 1));
        }
    }

    // 计算每条延迟线的反射次数和衰减量
    std::vector<float> outputSignal(inputSignal.size());
    for (int i = 0; i < reflectionLinePositions.size(); i += 2) {
        float totalDelay = 0.0f;
        float totalAmplitude = 1.0f;
        for (int j = 0; j < inputSignal.size(); j++) {
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
                outputSignal[j] += inputSignal[j - delay] * amplitude;
            }
        }
        // 输出每条延迟线的延迟量和衰减量
        std::cout << "Reflection line " << i / 2 << " delay: " << totalDelay << ", amplitude: " << totalAmplitude << std::endl;
    }

    return outputSignal;
}
