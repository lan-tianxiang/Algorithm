/*
    FILE NAME: audio_gain.c
    PURPOSE: Implementation of gain effect
    DATE: 2024/11/19
    AUTHOR: lan-tianxiang
*/

#include "hd_audio_gain.h"
#include "hd_audio_effect_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static effect_result_t gain_process(effect_handle_t* self, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer);
static effect_result_t gain_command(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                                    uint32_t* replySize, void* replyData);
static effect_result_t gain_create(effect_handle_t** out, const effect_descriptor_t* descriptor);
static effect_result_t gain_release(effect_handle_t* self);
static effect_result_t gain_get_descriptor(effect_handle_t* self, effect_descriptor_t* out);

const effect_interface_t g_gain_interface = {
    .process = gain_process,
    .command = gain_command,
    .create = gain_create,
    .release = gain_release,
    .get_descriptor = gain_get_descriptor,
};

// 增益效果结构体
typedef struct {
    effect_handle_t handle;
    float gain;
} gain_effect_t;

// 16位有符号整数格式处理函数
inline static effect_result_t process_s16_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    int16_t* inSamples = (int16_t*)inBuffer->buffer;
    int16_t* outSamples = (int16_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->channelCount;
    
    for (size_t i = 0; i < frames * channels; ++i) {
        int32_t v = (int32_t)(inSamples[i] * effect->gain);
        if (v > INT16_MAX) v = INT16_MAX;
        if (v < INT16_MIN) v = INT16_MIN;
        outSamples[i] = (int16_t)v;
    }
    
    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->sampleRate = inBuffer->sampleRate;
    outBuffer->channelCount = inBuffer->channelCount;
    outBuffer->stride = inBuffer->stride;
    outBuffer->format = inBuffer->format;
    
    return EFFECT_OK;
}

// 24位有符号整数格式处理函数
inline static effect_result_t process_s24_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    // 假设24位数据以32位整数形式存储，低24位有效
    int32_t* inSamples = (int32_t*)inBuffer->buffer;
    int32_t* outSamples = (int32_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->channelCount;
    
    for (size_t i = 0; i < frames * channels; ++i) {
        int64_t v = (int64_t)(inSamples[i]) * effect->gain;
        // 保持24位范围 (-2^23 到 2^23-1)
        if (v > 8388607) v = 8388607;
        if (v < -8388608) v = -8388608;
        outSamples[i] = (int32_t)v;
    }
    
    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->sampleRate = inBuffer->sampleRate;
    outBuffer->channelCount = inBuffer->channelCount;
    outBuffer->stride = inBuffer->stride;
    outBuffer->format = inBuffer->format;
    
    return EFFECT_OK;
}

// 32位有符号整数格式处理函数
inline static effect_result_t process_s32_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    int32_t* inSamples = (int32_t*)inBuffer->buffer;
    int32_t* outSamples = (int32_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->channelCount;
    
    for (size_t i = 0; i < frames * channels; ++i) {
        int64_t v = (int64_t)(inSamples[i]) * effect->gain;
        if (v > INT32_MAX) v = INT32_MAX;
        if (v < INT32_MIN) v = INT32_MIN;
        outSamples[i] = (int32_t)v;
    }
    
    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->sampleRate = inBuffer->sampleRate;
    outBuffer->channelCount = inBuffer->channelCount;
    outBuffer->stride = inBuffer->stride;
    outBuffer->format = inBuffer->format;
    
    return EFFECT_OK;
}

// 32位浮点格式处理函数
inline static effect_result_t process_float_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    float* inSamples = (float*)inBuffer->buffer;
    float* outSamples = (float*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->channelCount;
    
    for (size_t i = 0; i < frames * channels; ++i) {
        float v = inSamples[i] * effect->gain;
        // 限制在 [-1.0, 1.0] 范围内，避免浮点溢出
        if (v > 1.0f) v = 1.0f;
        if (v < -1.0f) v = -1.0f;
        outSamples[i] = v;
    }
    
    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->sampleRate = inBuffer->sampleRate;
    outBuffer->channelCount = inBuffer->channelCount;
    outBuffer->stride = inBuffer->stride;
    outBuffer->format = inBuffer->format;
    
    return EFFECT_OK;
}

// 64位浮点格式处理函数
inline static effect_result_t process_double_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    double* inSamples = (double*)inBuffer->buffer;
    double* outSamples = (double*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->channelCount;
    
    for (size_t i = 0; i < frames * channels; ++i) {
        double v = inSamples[i] * effect->gain;
        // 限制在 [-1.0, 1.0] 范围内
        if (v > 1.0) v = 1.0;
        if (v < -1.0) v = -1.0;
        outSamples[i] = v;
    }
    
    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->sampleRate = inBuffer->sampleRate;
    outBuffer->channelCount = inBuffer->channelCount;
    outBuffer->stride = inBuffer->stride;
    outBuffer->format = inBuffer->format;
    
    return EFFECT_OK;
}

// 缓冲区内容复制函数
inline static effect_result_t copy_buffer_content(audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    // 如果输入输出缓冲区不是同一个，复制数据
    if (inBuffer->buffer != outBuffer->buffer) {
        size_t dataSize = inBuffer->frameCount * inBuffer->channelCount * inBuffer->stride;
        
        // 检查缓冲区大小是否足够
        if (dataSize == 0) {
            return EFFECT_BAD_VALUE;
        }
        
        memcpy(outBuffer->buffer, inBuffer->buffer, dataSize);
    }
    
    return EFFECT_OK;
}

static effect_result_t gain_process(effect_handle_t* self, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    if (!self || !inBuffer || !outBuffer) return EFFECT_BAD_VALUE;
    
    gain_effect_t* effect = (gain_effect_t*)self;
    
    // 如果效果被禁用，直接复制缓冲区参数并返回
    if (!effect->handle.enabled) {
        effect_result_t result = copy_buffer_content(inBuffer, outBuffer);
        if (result != EFFECT_OK) {
            return result;
        }
        
        // 复制所有缓冲区参数
        outBuffer->frameCount = inBuffer->frameCount;
        outBuffer->sampleRate = inBuffer->sampleRate;
        outBuffer->channelCount = inBuffer->channelCount;
        outBuffer->stride = inBuffer->stride;
        outBuffer->format = inBuffer->format;
        return EFFECT_OK;
    }
    
    // 根据不同的PCM格式调用对应的处理函数
    switch (inBuffer->format) {
        case PCM_FORMAT_S16_LE:
            return process_s16_le(effect, inBuffer, outBuffer);
        case PCM_FORMAT_S24_LE:
            return process_s24_le(effect, inBuffer, outBuffer);
        case PCM_FORMAT_S32_LE:
            return process_s32_le(effect, inBuffer, outBuffer);
        case PCM_FORMAT_FLOAT_LE:
            return process_float_le(effect, inBuffer, outBuffer);
        case PCM_FORMAT_DOUBLE_LE:
            return process_double_le(effect, inBuffer, outBuffer);
        default:
            // 不支持的格式，返回错误
            return EFFECT_BAD_VALUE;
    }
}

static effect_result_t gain_command(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                                    uint32_t* replySize, void* replyData) {
    if (!self) return EFFECT_BAD_VALUE;
    gain_effect_t* g = (gain_effect_t*)self;
    switch (cmdCode) {
        case EFFECT_CMD_SET_PARAM:
            if (cmdSize >= sizeof(float) && cmdData) {
                float v;
                memcpy(&v, cmdData, sizeof(float));
                g->gain = v;
                return EFFECT_OK;
            }
            return EFFECT_BAD_VALUE;
        case EFFECT_CMD_GET_PARAM:
            if (replySize && replyData && *replySize >= sizeof(float)) {
                memcpy(replyData, &g->gain, sizeof(float));
                *replySize = sizeof(float);
                return EFFECT_OK;
            }
            return EFFECT_BAD_VALUE;
        case EFFECT_CMD_ENABLE:
            g->handle.enabled = 1;
            return EFFECT_OK;
        case EFFECT_CMD_DISABLE:
            g->handle.enabled = 0;
            return EFFECT_OK;
        default:
            return EFFECT_ERROR;
    }
}

static effect_result_t gain_create(effect_handle_t** out, const effect_descriptor_t* descriptor) {
    if (!out) return EFFECT_BAD_VALUE;
    gain_effect_t* g = (gain_effect_t*)calloc(1, sizeof(*g));
    if (!g) return EFFECT_NO_MEMORY;
    g->handle.itfe = &g_gain_interface;
    g->handle.enabled = 1;
    g->gain = 1.0f;
    if (descriptor) g->handle.descriptor = *descriptor;
    *out = &g->handle;
    return EFFECT_OK;
}

static effect_result_t gain_release(effect_handle_t* self) {
    if (!self) return EFFECT_BAD_VALUE;
    gain_effect_t* g = (gain_effect_t*)self;
    free(g);
    return EFFECT_OK;
}

static effect_result_t gain_get_descriptor(effect_handle_t* self, effect_descriptor_t* out) {
    if (!self || !out) return EFFECT_BAD_VALUE;
    gain_effect_t* g = (gain_effect_t*)self;
    *out = g->handle.descriptor;
    return EFFECT_OK;
}

// Exported plugin symbols (ABI): plugin loader will look for these names
// create and release functions
__attribute__((visibility("default")))
effect_result_t effect_create_gain(effect_handle_t** out, const effect_descriptor_t* desc) {
    return gain_create(out, desc);
}

__attribute__((visibility("default")))
effect_result_t effect_release_gain(effect_handle_t* h) {
    return gain_release(h);
}