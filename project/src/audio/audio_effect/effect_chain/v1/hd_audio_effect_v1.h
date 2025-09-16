/*
    FILE NAME: hd_audio_effect.h
    PURPOSE: Header file for audio effect module (AOSP-like API compatibility)
    DATE: 2025/9/16
    AUTHOR: nolan.lan (adapted)
*/

#ifndef HD_AUDIO_EFFECT_H
#define HD_AUDIO_EFFECT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// ------------- 基本类型与返回值 --------------
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EFFECT_OK = 0,           /* 操作成功 */
    EFFECT_ERROR = -1,       /* 通用错误 */
    EFFECT_BAD_VALUE = -2,   /* 参数值无效 */
    EFFECT_NO_MEMORY = -3,   /* 内存不足 */
    EFFECT_NO_INIT = -4      /* 未初始化 */
} effect_result_t;

// ------------- UUID (128-bit) --------------
typedef struct { uint8_t data[16]; } effect_uuid_t;

// ------------- descriptor --------------
typedef struct {
    effect_uuid_t type;        // effect type UUID
    effect_uuid_t uuid;        // instance UUID
    uint32_t apiVersion;       // API 版本
    uint32_t flags;            // 标志位
    char     name[64];         // 名称
    char     implementor[64];  // 实现者
} effect_descriptor_t;

// ------------- PCM / buffer --------------
typedef enum {
    PCM_FORMAT_UNKNOWN = 0,
    PCM_FORMAT_S16_LE,
    PCM_FORMAT_S24_LE,
    PCM_FORMAT_S32_LE,
    PCM_FORMAT_FLOAT_LE,
    PCM_FORMAT_DOUBLE_LE,
} pcm_format_t;

typedef struct {
    void* buffer;         // 指向样本数据
    uint32_t frameCount;  // 帧数
    uint32_t sampleRate;  // 采样率
    uint32_t channelCount;// 通道数
    uint32_t stride;      // 每帧字节数
    pcm_format_t format;  // 格式
} audio_buffer_t;

// ------------- command / param / config --------------
// command 常量（兼容 AOSP 风格）
typedef enum {
    EFFECT_CMD_INIT = 0,     /* 初始化命令 */
    EFFECT_CMD_SET_PARAM = 1,/* 设置参数 */
    EFFECT_CMD_GET_PARAM = 2,/* 获取参数 */
    EFFECT_CMD_ENABLE = 3,   /* 启用效果 */
    EFFECT_CMD_DISABLE = 4,  /* 禁用效果 */
    EFFECT_CMD_SET_CONFIG = 5,/* 设置配置 */
    EFFECT_CMD_RESET = 6     /* 重置效果 */
} effect_cmd_id_t;

// 参数结构（灵活字节流）
typedef struct {
    uint32_t param_size; // 参数标识或长度
    uint32_t value_size; // 值长度
    uint8_t  data[];     // param id + value bytes
} effect_param_t;

typedef struct {
    uint32_t sampleRate;
    uint32_t channelCount;
    pcm_format_t format;
} effect_config_t;

// ------------- effect interface & handle --------------
typedef struct effect_handle_s effect_handle_t;

typedef struct {
    effect_result_t (*process)(effect_handle_t* self, audio_buffer_t* in, audio_buffer_t* out);
    effect_result_t (*command)(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                               uint32_t* replySize, void* replyData);
    effect_result_t (*create)(effect_handle_t** self, const effect_descriptor_t* descriptor);
    effect_result_t (*release)(effect_handle_t* self);
    effect_result_t (*get_descriptor)(effect_handle_t* self, effect_descriptor_t* descriptor);
} effect_interface_t;

struct effect_handle_s {
    const effect_interface_t* itfe;
    effect_descriptor_t descriptor;
    int enabled;
    void* instance_data; // 可由具体 effect 使用
};

// ------------- 链管理 --------------
typedef struct effect_node_s {
    effect_handle_t* effect;
    struct effect_node_s* next;
} effect_node_t;

typedef struct {
    effect_node_t* head;
} effect_chain_t;

// ------------- 函数声明（ABI/导出） --------------
// create/destroy chain
effect_chain_t* create_effect_chain(void);
effect_result_t destroy_effect_chain(effect_chain_t* chain);

// add/remove
effect_result_t add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect);
effect_result_t remove_effect_from_chain(effect_chain_t* chain, effect_handle_t* effect);

// 处理（串行执行整个链）
effect_result_t process_audio_chain(effect_chain_t* chain, audio_buffer_t* in, audio_buffer_t* out);

// enable/disable a single effect
effect_result_t set_effect_enabled(effect_chain_t* chain, effect_handle_t* effect, int enabled);

// query list of effects; out_list should point to an array of effect_handle_t* with capacity max
effect_result_t get_chain_effects(effect_chain_t* chain, effect_handle_t** out_list, size_t max, size_t* out_count);

// Optional: chain-level convenience API to send command to all effects
effect_result_t chain_send_command(effect_chain_t* chain, uint32_t cmdCode, uint32_t cmdSize, void* cmdData);

#ifdef __cplusplus
}
#endif

#endif // HD_AUDIO_EFFECT_H