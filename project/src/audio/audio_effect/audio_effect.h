#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 音效描述符
typedef struct {
    uint64_t type;           // 音效类型的唯一标识符
    uint64_t uuid;           // 音效实现的唯一标识符
    uint32_t apiVersion;     // API 版本
    uint32_t flags;          // 标志位，指示音效的特性
    char     name[64];       // 音效名称
    char     implementor[64];// 实现者
} effect_descriptor_t;

// 音频缓冲区
typedef struct {
    size_t frameCount;  // 音频帧的数量
    void*  raw;         // 指向音频数据的指针
} audio_buffer_t;

// 前向声明
typedef struct effect_handle_s effect_handle_t;

// 音效接口
typedef struct {
    int (*process)(effect_handle_t* self, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer);
    int (*command)(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                   uint32_t* replySize, void* replyData);
    int (*create)(effect_handle_t* self);
    int (*release)(effect_handle_t* self);
    int (*get_descriptor)(effect_handle_t* self, effect_descriptor_t* descriptor);
} effect_interface_t;

// 音效句柄
struct effect_handle_s {
    const effect_interface_t* itfe; // 指向音效接口的指针
    // 可以在此添加效果相关的数据
};

// 音效链节点
typedef struct effect_node_s {
    effect_handle_t* effect;         // 指向音效实例的指针
    struct effect_node_s* next;      // 指向下一个音效节点
} effect_node_t;

// 音效链
typedef struct {
    effect_node_t* head;  // 音效链的头部
} effect_chain_t;

// 函数声明
effect_chain_t* create_effect_chain();
void destroy_effect_chain(effect_chain_t* chain);
void add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect);
void process_audio_chain(effect_chain_t* chain, audio_buffer_t* inBuffer, audio_buffer_t* outBuffer);

#endif // AUDIO_EFFECT_H