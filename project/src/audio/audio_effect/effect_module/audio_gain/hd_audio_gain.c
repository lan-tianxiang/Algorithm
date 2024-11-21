/*
    FILE NAME: audio_gain.c
    PURPOSE: Implementation of gain effect
    DATE: 2024/11/19
    AUTHOR: lan-tianxiang
*/

#include "hd_audio_gain.h"

// 处理函数
int gain_process(effect_handle_t* self, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    gain_effect_t* effect = (gain_effect_t*)self;
    int16_t* inSamples = (int16_t*)inBuffer->raw;
    int16_t* outSamples = (int16_t*)outBuffer->raw;
    for (size_t i = 0; i < inBuffer->frameCount; ++i) {
        outSamples[i] = (int16_t)(inSamples[i] * effect->gain);
    }
    return 0;
}

// 其他接口函数（可根据需要实现）
int gain_command(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                 uint32_t* replySize, void* replyData) {
    return 0;
}

int gain_create(effect_handle_t* self) {
    return 0;
}

int gain_release(effect_handle_t* self) {
    return 0;
}

int gain_get_descriptor(effect_handle_t* self, effect_descriptor_t* descriptor) {
    return 0;
}

// 定义增益效果的接口
const effect_interface_t gain_interface = {
    .process = gain_process,
    .command = gain_command,
    .create = gain_create,
    .release = gain_release,
    .get_descriptor = gain_get_descriptor
};

// 创建增益效果实例
effect_handle_t* create_gain_effect(float gain) {
    gain_effect_t* effect = (gain_effect_t*)malloc(sizeof(gain_effect_t));
    effect->handle.itfe = &gain_interface;
    effect->gain = gain;
    effect->handle.itfe->create(&effect->handle);
    return &effect->handle;
}