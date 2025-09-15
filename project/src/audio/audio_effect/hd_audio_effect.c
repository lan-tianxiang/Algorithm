/*
    FILE NAME: audio_effect.c
    PURPOSE: Implementation of audio effect
    DATE: 2025/9/15
    AUTHOR: nolan.lan
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

// 从链表移除音效
void remove_effect_from_chain(effect_chain_t* chain, effect_handle_t* effect) {
    if (chain == NULL || chain->head == NULL) {
        return;
    }
    
    // 如果要移除的是头节点
    if (chain->head->effect == effect) {
        effect_node_t* temp = chain->head;
        chain->head = chain->head->next;
        free(temp);
        return;
    }
    
    // 查找要移除的节点
    effect_node_t* current = chain->head;
    while (current->next != NULL && current->next->effect != effect) {
        current = current->next;
    }
    
    // 如果找到了要移除的节点
    if (current->next != NULL) {
        effect_node_t* temp = current->next;
        current->next = current->next->next;
        free(temp);
    }
}

// 处理音频数据
void process_audio_chain(effect_chain_t* chain, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer) {
    if (chain == NULL || chain->head == NULL) {
        // 如果音效链为空，直接复制输入到输出
        if (inBuffer->bitWidth == 24) {
            // 处理24位采样
            uint32_t bufferSize = inBuffer->frameCount * (inBuffer->bitWidth / 8);
            memcpy(outBuffer->raw, inBuffer->raw, bufferSize);
            outBuffer->frameCount = inBuffer->frameCount;
        } else if (inBuffer->bitWidth == 16) {
            // 处理16位采样
            memcpy(outBuffer->raw, inBuffer->raw, inBuffer->frameCount * sizeof(int16_t));
            outBuffer->frameCount = inBuffer->frameCount;
        }
        return;
    }
    
    effect_node_t* node = chain->head;
    audio_buffer_t* currentBuffer = inBuffer;
    audio_buffer_t intermediateBuffer;
    intermediateBuffer.sampleRate = inBuffer->sampleRate;
    intermediateBuffer.channelNum = inBuffer->channelNum;
    intermediateBuffer.bitWidth = inBuffer->bitWidth;
    
    while (node != NULL) {
        // 创建用于存储输出的中间缓冲区
        intermediateBuffer.frameCount = currentBuffer->frameCount;
        if (intermediateBuffer.bitWidth == 24) {
            // 24位采样（每样本3字节）
            intermediateBuffer.raw = malloc(intermediateBuffer.frameCount * 3);
        } else if (intermediateBuffer.bitWidth == 16) {
            // 16位采样
            intermediateBuffer.raw = malloc(intermediateBuffer.frameCount * sizeof(int16_t));
        } else {
            // 默认处理
            intermediateBuffer.raw = malloc(intermediateBuffer.frameCount * sizeof(int16_t));
        }
        
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
    if (currentBuffer->bitWidth == 24) {
        uint32_t bufferSize = currentBuffer->frameCount * 3;
        memcpy(outBuffer->raw, currentBuffer->raw, bufferSize);
    } else if (currentBuffer->bitWidth == 16) {
        memcpy(outBuffer->raw, currentBuffer->raw, currentBuffer->frameCount * sizeof(int16_t));
    }
    outBuffer->frameCount = currentBuffer->frameCount;
    outBuffer->sampleRate = currentBuffer->sampleRate;
    outBuffer->channelNum = currentBuffer->channelNum;
    outBuffer->bitWidth = currentBuffer->bitWidth;
    
    // 释放最后的中间缓冲区
    free(currentBuffer->raw);
}