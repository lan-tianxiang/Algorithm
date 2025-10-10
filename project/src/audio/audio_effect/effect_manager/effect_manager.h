/*
 * effect_manager.h
 * Simple manager to create/destroy/list effects from effect_store and attach to chains
 */
#ifndef EFFECT_MANAGER_H
#define EFFECT_MANAGER_H

#include "hd_audio_effect_wrapper.h"
#include "effect_store.h"

// 创建特定业务类型的效果链
typedef struct effect_chain_wrapper {
    effect_chain_t* chain;
    effect_store_bussness_id_t bus_id;
} effect_chain_wrapper_t;

// 函数声明
effect_chain_wrapper_t* create_bussness_effect_chain(effect_store_bussness_id_t bus_id);
effect_result_t process_bussness_audio(effect_chain_wrapper_t* wrapper, audio_buffer_t* in, audio_buffer_t* out);
effect_result_t destroy_bussness_effect_chain(effect_chain_wrapper_t* wrapper);
effect_result_t add_effect_from_store(effect_chain_t* chain,
                                     effect_store_bussness_id_t bus_id, 
                                     size_t effect_index);
                                      
// 新增：基于UUID的效果移除函数声明
effect_result_t remove_effect_by_uuid_from_chain(effect_chain_wrapper_t* wrapper,
                                                const effect_uuid_t* type_uuid, 
                                                size_t remove_index);

#endif // EFFECT_MANAGER_H