
/*
 * effect_manager.c
 * Minimal manager for effects using effect_store arrays and hd_audio_effect wrapper APIs
 */

#include "effect_manager.h"
#include <stdlib.h>
#include <string.h>

effect_chain_wrapper_t* create_bussness_effect_chain(effect_store_bussness_id_t bus_id) {
    // 1. 获取对应的效果存储
    effect_store_bussness_t* store_bus = get_effect_store_bussness(bus_id);
    if (!store_bus) return NULL;
    
    // 2. 创建效果链
    effect_chain_t* chain = audio_create_effect_chain();
    if (!chain) return NULL;
    
    // 3. 将存储中的所有效果添加到效果链中
    for (size_t i = 0; i < store_bus->count; i++) {
        effect_store_t* effect_store = &store_bus->store[i];
        
        // 创建效果实例
        effect_handle_t* effect = NULL;
        // 假设使用默认描述符，实际应用中应使用正确的描述符
        effect_descriptor_t desc = {0};
        
        // 使用效果接口创建效果实例
        if (effect_store->interface && effect_store->interface->create) {
            effect_result_t result = effect_store->interface->create(&effect, &desc);
            if (result != EFFECT_OK || !effect) {
                // 创建失败，清理已创建的效果链
                audio_destroy_effect_chain(chain);
                return NULL;
            }
            
            // 启用效果
            effect->enabled = 1;
            
            // 添加到效果链
            if (audio_add_effect_to_chain(chain, effect) != EFFECT_OK) {
                // 添加失败，释放效果并清理效果链
                if (effect->itfe && effect->itfe->release) {
                    effect->itfe->release(effect);
                }
                audio_destroy_effect_chain(chain);
                return NULL;
            }
        }
    }
    
    // 4. 返回效果链
    effect_chain_wrapper_t* wrapper = malloc(sizeof(effect_chain_wrapper_t));
    if (!wrapper) return NULL;
    
    wrapper->chain = chain;
    wrapper->bus_id = bus_id;
    
    return wrapper;
}

// 处理特定业务的音频数据
effect_result_t process_bussness_audio(effect_chain_wrapper_t* wrapper, audio_buffer_t* in, audio_buffer_t* out) {
    if (!wrapper || !wrapper->chain || !in || !out) return EFFECT_BAD_VALUE;
    return audio_process_audio_chain(wrapper->chain, in, out);
}

// 销毁业务效果链
effect_result_t destroy_bussness_effect_chain(effect_chain_wrapper_t* wrapper) {
    if (!wrapper || !wrapper->chain) return EFFECT_BAD_VALUE;
    effect_result_t result = EFFECT_OK;
    result = audio_destroy_effect_chain(wrapper->chain);
    if (result != EFFECT_OK) {
        return result;
    }
    free(wrapper);
    return EFFECT_OK;
}

// 根据索引获取并添加特定效果到链中
effect_result_t add_effect_from_store(effect_chain_t* chain, 
                                     effect_store_bussness_id_t bus_id, 
                                     size_t effect_index) {
    // 1. 获取效果存储
    effect_store_bussness_t* store_bus = get_effect_store_bussness(bus_id);
    if (!store_bus || effect_index >= store_bus->count) return EFFECT_BAD_VALUE;
    
    // 2. 获取效果接口
    effect_store_t* effect_store = &store_bus->store[effect_index];
    
    // 3. 创建效果实例
    effect_handle_t* effect = NULL;
    effect_descriptor_t desc = {0};
    effect_result_t result = EFFECT_BAD_VALUE;
    
    if (effect_store->interface && effect_store->interface->create) {
        result = effect_store->interface->create(&effect, &desc);
        if (result != EFFECT_OK || !effect) {
            return result;
        }
        
        // 启用效果
        effect->enabled = 1;
        
        // 4. 添加到效果链
        result = audio_add_effect_to_chain(chain, effect);
        if (result != EFFECT_OK) {
            if (effect->itfe && effect->itfe->release) {
                effect->itfe->release(effect);
            }
        }
    }
    
    return result;
}

// 从效果链中移除指定类型UUID的效果
// 参数:
// - chain: 效果链指针
// - type_uuid: 效果类型UUID
// - remove_index: 要移除的同类型效果的索引 (从0开始计数)
// 返回值:
// - EFFECT_OK: 成功
// - EFFECT_BAD_VALUE: 参数无效
// - EFFECT_ERROR: 移除失败或未找到指定索引的效果
effect_result_t remove_effect_by_uuid_from_chain(effect_chain_wrapper_t* wrapper,
                                                const effect_uuid_t* type_uuid, 
                                                size_t remove_index) {
    if (!wrapper || !wrapper->chain || !type_uuid) return EFFECT_BAD_VALUE;
    effect_chain_t* chain = wrapper->chain;
    // 首先获取链中所有效果的数量
    size_t effect_count = 0;
    if (audio_get_chain_effects(chain, NULL, 0, &effect_count) != EFFECT_OK) {
        return EFFECT_ERROR;
    }
    
    if (effect_count == 0) return EFFECT_ERROR;
    
    // 分配空间存储效果句柄
    effect_handle_t** effect_list = (effect_handle_t**)malloc(effect_count * sizeof(effect_handle_t*));
    if (!effect_list) return EFFECT_NO_MEMORY;
    
    // 获取效果链中的所有效果
    size_t actual_count = 0;
    if (audio_get_chain_effects(chain, effect_list, effect_count, &actual_count) != EFFECT_OK) {
        free(effect_list);
        return EFFECT_ERROR;
    }
    
    // 查找并移除特定索引的指定UUID类型的效果
    size_t type_index = 0;
    effect_handle_t* target_effect = NULL;
    
    for (size_t i = 0; i < actual_count; i++) {
        effect_handle_t* effect = effect_list[i];
        if (effect) {
            effect_descriptor_t desc;
            if (effect->itfe && effect->itfe->get_descriptor && 
                effect->itfe->get_descriptor(effect, &desc) == EFFECT_OK &&
                effect_uuid_compare(&desc.type, type_uuid)) {
                
                if (type_index == remove_index) {
                    target_effect = effect;
                    break;
                }
                type_index++;
            }
        }
    }
    
    effect_result_t result = EFFECT_ERROR;
    if (target_effect) {
        // 从链中移除效果
        result = audio_remove_effect_from_chain(chain, target_effect);
    }
    
    free(effect_list);
    return result;
}