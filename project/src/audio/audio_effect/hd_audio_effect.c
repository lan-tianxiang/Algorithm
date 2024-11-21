/*
    FILE NAME: audio_effect.c
    PURPOSE: Implementation of audio effect
    DATE: 2024/11/18
    AUTHOR: lan-tianxiang
*/

#include "hd_audio_effect.h"

// 创建音效链
effect_chain_t* create_effect_chain() {
    effect_chain_t* chain = (effect_chain_t*)malloc(sizeof(effect_chain_t));
    chain->head = NULL;
    return chain;
}

// 销毁音效链
void destroy_effect_chain(effect_chain_t* chain) {
    effect_node_t* node = chain->head;
    while (node != NULL) {
        effect_node_t* next = node->next;
        // 释放音效实例
        node->effect->itfe->release(node->effect);
        // 释放音效句柄
        free(node->effect);
        // 释放节点
        free(node);
        node = next;
    }
    free(chain);
}

// 将音效添加到链表
void add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect) {
    effect_node_t* node = (effect_node_t*)malloc(sizeof(effect_node_t));
    node->effect = effect;
    node->next = chain->head;
    chain->head = node;
}

// 处理音频数据
void process_audio_chain(effect_chain_t* chain, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    effect_node_t* node = chain->head;
    audio_buffer_t* currentBuffer = inBuffer;
    audio_buffer_t intermediateBuffer;
    while (node != NULL) {
        // 创建用于存储输出的中间缓冲区
        intermediateBuffer.frameCount = currentBuffer->frameCount;
        intermediateBuffer.raw = malloc(currentBuffer->frameCount * sizeof(int16_t)); // 假设 16 位采样
        // 处理音频数据
        node->effect->itfe->process(node->effect, currentBuffer, &intermediateBuffer);
        // 释放上一个缓冲区（如果不是输入缓冲区）
        if (currentBuffer != inBuffer) {
            free(currentBuffer->raw);
        }
        // 准备下一个效果处理
        currentBuffer = &intermediateBuffer;
        node = node->next;
    }
    // 将最终输出复制到 outBuffer
    memcpy(outBuffer->raw, currentBuffer->raw, currentBuffer->frameCount * sizeof(int16_t));
    outBuffer->frameCount = currentBuffer->frameCount;
    // 释放最后的中间缓冲区
    free(currentBuffer->raw);
}