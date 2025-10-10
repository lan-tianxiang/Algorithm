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

// 定义增益效果的标准UUID
static const effect_uuid_t GAIN_EFFECT_UUID = {
    .data = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x01
    }
};

const effect_interface_t g_gain_interface = {
    .process = gain_process,
    .command = gain_command,
    .create = gain_create,
    .release = gain_release,
    .get_descriptor = gain_get_descriptor,
};

// const effect_handle_t g_effect_handle_rodata = {
//     .itfe = &g_gain_interface,
//     .descriptor = {
//         .type = GAIN_EFFECT_UUID,
//         .uuid = GAIN_EFFECT_UUID, // 实例UUID可与类型UUID相同
//         .flags = EFFECT_FLAG_TYPE_INSERT | EFFECT_FLAG_INSERT_FIRST,
//         .cpuLoad = 10, // 假设值
//         .memoryUsage = 1024, // 假设值
//         .name = "HD Audio Gain Effect",
//         .implementor = "HD Audio Team"
//     },
//     .enabled = 0,
//     .instance_data = NULL,
// };

// 增益效果结构体
typedef struct {
    effect_handle_t handle;
    float gain;
} gain_effect_t;

// 16位有符号整数格式处理函数
static effect_result_t process_s16_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    int16_t* inSamples = (int16_t*)inBuffer->buffer;
    int16_t* outSamples = (int16_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->config.channelCount;

    for (size_t i = 0; i < frames * channels; ++i) {
        int32_t v = (int32_t)(inSamples[i] * effect->gain);
        if (v > INT16_MAX) v = INT16_MAX;
        if (v < INT16_MIN) v = INT16_MIN;
        outSamples[i] = (int16_t)v;
    }

    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->config.sampleRate = inBuffer->config.sampleRate;
    outBuffer->config.channelCount = inBuffer->config.channelCount;
    outBuffer->config.stride = inBuffer->config.stride;
    outBuffer->config.format = inBuffer->config.format;

    return EFFECT_OK;
}

// 24位有符号整数格式处理函数
static effect_result_t process_s24_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    // 假设24位数据以32位整数形式存储，低24位有效
    int32_t* inSamples = (int32_t*)inBuffer->buffer;
    int32_t* outSamples = (int32_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->config.channelCount;

    for (size_t i = 0; i < frames * channels; ++i) {
        int64_t v = (int64_t)(inSamples[i]) * effect->gain;
        // 保持24位范围 (-2^23 到 2^23-1)
        if (v > 8388607) v = 8388607;
        if (v < -8388608) v = -8388608;
        outSamples[i] = (int32_t)v;
    }

    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->config.sampleRate = inBuffer->config.sampleRate;
    outBuffer->config.channelCount = inBuffer->config.channelCount;
    outBuffer->config.stride = inBuffer->config.stride;
    outBuffer->config.format = inBuffer->config.format;

    return EFFECT_OK;
}

// 32位有符号整数格式处理函数
static effect_result_t process_s32_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    int32_t* inSamples = (int32_t*)inBuffer->buffer;
    int32_t* outSamples = (int32_t*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->config.channelCount;

    for (size_t i = 0; i < frames * channels; ++i) {
        int64_t v = (int64_t)(inSamples[i]) * effect->gain;
        if (v > INT32_MAX) v = INT32_MAX;
        if (v < INT32_MIN) v = INT32_MIN;
        outSamples[i] = (int32_t)v;
    }

    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->config.sampleRate = inBuffer->config.sampleRate;
    outBuffer->config.channelCount = inBuffer->config.channelCount;
    outBuffer->config.stride = inBuffer->config.stride;
    outBuffer->config.format = inBuffer->config.format;

    return EFFECT_OK;
}

// 32位浮点格式处理函数
static effect_result_t process_float_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    float* inSamples = (float*)inBuffer->buffer;
    float* outSamples = (float*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->config.channelCount;

    for (size_t i = 0; i < frames * channels; ++i) {
        float v = inSamples[i] * effect->gain;
        // 限制在 [-1.0, 1.0] 范围内，避免浮点溢出
        // if (v > 1.0f) v = 1.0f;
        // if (v < -1.0f) v = -1.0f;
        outSamples[i] = v;
    }

    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->config.sampleRate = inBuffer->config.sampleRate;
    outBuffer->config.channelCount = inBuffer->config.channelCount;
    outBuffer->config.stride = inBuffer->config.stride;
    outBuffer->config.format = inBuffer->config.format;

    return EFFECT_OK;
}

// 64位浮点格式处理函数
static effect_result_t process_double_le(gain_effect_t* effect, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    double* inSamples = (double*)inBuffer->buffer;
    double* outSamples = (double*)outBuffer->buffer;
    size_t frames = inBuffer->frameCount;
    size_t channels = inBuffer->config.channelCount;

    for (size_t i = 0; i < frames * channels; ++i) {
        double v = inSamples[i] * effect->gain;
        // 限制在 [-1.0, 1.0] 范围内
        // if (v > 1.0) v = 1.0;
        // if (v < -1.0) v = -1.0;
        outSamples[i] = v;
    }

    // 复制缓冲区参数
    outBuffer->frameCount = inBuffer->frameCount;
    outBuffer->config.sampleRate = inBuffer->config.sampleRate;
    outBuffer->config.channelCount = inBuffer->config.channelCount;
    outBuffer->config.stride = inBuffer->config.stride;
    outBuffer->config.format = inBuffer->config.format;

    return EFFECT_OK;
}

static effect_result_t gain_process(effect_handle_t* self, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    if (!self || !inBuffer || !outBuffer) return EFFECT_BAD_VALUE;

    gain_effect_t* effect = (gain_effect_t*)self;

    // 根据不同的PCM格式调用对应的处理函数
    switch (inBuffer->config.format) {
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

// 修改gain_create函数，设置默认的效果类型UUID
static effect_result_t gain_create(effect_handle_t** out, const effect_descriptor_t* descriptor) {
    if (!out) return EFFECT_BAD_VALUE;
    gain_effect_t* g = (gain_effect_t*)calloc(1, sizeof(*g));
    if (!g) return EFFECT_NO_MEMORY;
    g->handle.itfe = &g_gain_interface;
    g->handle.enabled = 1;
    g->gain = 1.0f;
    
    // 如果提供了描述符，使用它；否则设置默认的增益效果UUID
    if (descriptor) {
        g->handle.descriptor = *descriptor;
    } else {
        // 设置默认的效果类型UUID
        g->handle.descriptor.type = GAIN_EFFECT_UUID;
        // 生成一个简单的实例UUID（实际应用中应该使用更健壮的生成方法）
        memset(&g->handle.descriptor.uuid, 0, sizeof(effect_uuid_t));
        g->handle.descriptor.uuid.data[0] = rand();
        
        // 设置其他默认值
        g->handle.descriptor.apiVersion = 1;
        g->handle.descriptor.flags = 0;
        strncpy(g->handle.descriptor.name, "Gain Effect", sizeof(g->handle.descriptor.name) - 1);
        strncpy(g->handle.descriptor.implementor, "Example Inc.", sizeof(g->handle.descriptor.implementor) - 1);
    }
    
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