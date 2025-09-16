/*
    FILE NAME: hd_audio_effect.c
    PURPOSE: Implementation of audio effect chain with AOSP-like API
    DATE: 2025/9/16
    AUTHOR: nolan.lan (adapted)
*/

#include "hd_audio_effect_v1.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>

// Simple thread-safety for chain operations
typedef struct {
    effect_chain_t chain;
    pthread_mutex_t lock;
} effect_chain_wrapper_t;

static effect_chain_wrapper_t* allocate_chain_wrapper(void) {
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)calloc(1, sizeof(*w));
    if (!w) return NULL;
    pthread_mutex_init(&w->lock, NULL);
    w->chain.head = NULL;
    return w;
}

static void free_chain_wrapper(effect_chain_wrapper_t* w) {
    if (!w) return;
    pthread_mutex_destroy(&w->lock);
    free(w);
}

// Create chain
effect_chain_t* create_effect_chain(void) {
    effect_chain_wrapper_t* w = allocate_chain_wrapper();
    if (!w) return NULL;
    return &w->chain;
}

// Destroy chain
effect_result_t destroy_effect_chain(effect_chain_t* chain) {
    if (!chain) return EFFECT_BAD_VALUE;
    // chain pointer is actually inside wrapper
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;

    pthread_mutex_lock(&w->lock);
    effect_node_t* cur = w->chain.head;
    while (cur) {
        effect_node_t* next = cur->next;
        // try to release effect instances if provided
        if (cur->effect && cur->effect->itfe && cur->effect->itfe->release) {
            cur->effect->itfe->release(cur->effect);
        }
        free(cur);
        cur = next;
    }
    w->chain.head = NULL;
    pthread_mutex_unlock(&w->lock);

    free_chain_wrapper(w);
    return EFFECT_OK;
}

// Add effect
effect_result_t add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect) {
    if (!chain || !effect) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;

    effect_node_t* node = (effect_node_t*)calloc(1, sizeof(*node));
    if (!node) return EFFECT_NO_MEMORY;
    node->effect = effect;
    node->next = NULL;  // 确保新节点的next指针为空

    pthread_mutex_lock(&w->lock);
    if (!w->chain.head) {
        w->chain.head = node;
    } else {
        effect_node_t* cur = w->chain.head;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = node;
    }
    pthread_mutex_unlock(&w->lock);
    return EFFECT_OK;
}

// Remove effect
effect_result_t remove_effect_from_chain(effect_chain_t* chain, effect_handle_t* effect) {
    if (!chain || !effect) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;

    pthread_mutex_lock(&w->lock);
    effect_node_t* cur = w->chain.head;
    effect_node_t* prev = NULL;
    while (cur) {
        if (cur->effect == effect) {
            if (prev) prev->next = cur->next;
            else w->chain.head = cur->next;
            // optionally release
            if (cur->effect && cur->effect->itfe && cur->effect->itfe->release) {
                cur->effect->itfe->release(cur->effect);
            }
            free(cur);
            pthread_mutex_unlock(&w->lock);
            return EFFECT_OK;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_mutex_unlock(&w->lock);
    return EFFECT_BAD_VALUE;
}

effect_result_t set_effect_enabled(effect_chain_t* chain, effect_handle_t* effect, int enabled) {
    if (!chain || !effect) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;
    pthread_mutex_lock(&w->lock);
    effect_node_t* cur = w->chain.head;
    while (cur) {
        if (cur->effect == effect) {
            cur->effect->enabled = enabled ? 1 : 0;
            pthread_mutex_unlock(&w->lock);
            return EFFECT_OK;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&w->lock);
    return EFFECT_BAD_VALUE;
}

effect_result_t get_chain_effects(effect_chain_t* chain, effect_handle_t** out_list, size_t max, size_t* out_count) {
    if (!chain || !out_list || !out_count) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;
    pthread_mutex_lock(&w->lock);
    size_t idx = 0;
    effect_node_t* cur = w->chain.head;
    while (cur && idx < max) {
        out_list[idx++] = cur->effect;
        cur = cur->next;
    }
    *out_count = idx;
    pthread_mutex_unlock(&w->lock);
    return EFFECT_OK;
}

// Process chain: in -> each effect -> out (simple serial processing)
effect_result_t process_audio_chain(effect_chain_t* chain, audio_buffer_t* in, audio_buffer_t* out) {
    if (!chain || !in || !out) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;

    // For simplicity, we will use a temporary buffer pointer swapping approach to support in-place processing.
    audio_buffer_t work_in = *in;
    audio_buffer_t work_out = *out;

    pthread_mutex_lock(&w->lock);
    effect_node_t* cur = w->chain.head;
    while (cur) {
        effect_handle_t* h = cur->effect;
        if (h && h->itfe && h->itfe->process && h->enabled) {
            effect_result_t r = h->itfe->process(h, &work_in, &work_out);
            if (r != EFFECT_OK) {
                pthread_mutex_unlock(&w->lock);
                return r;
            }
            // swap work_in and work_out for next effect (allow in-place)
            audio_buffer_t tmp = work_in;
            work_in = work_out;
            work_out = tmp;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&w->lock);

    // final output: copy resulting buffer metadata to out
    *out = work_in; // assume last result resides in work_in after swaps
    return EFFECT_OK;
}

// Simple example helper: set effect enabled/disabled via command
// Supports standard commands: INIT, ENABLE, DISABLE, SET_PARAM (simple copy)

// note: this is a minimal command dispatcher for chain-level commands
static effect_result_t dispatch_command_to_effect(effect_handle_t* h, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                                                 uint32_t* replySize, void* replyData) {
    if (!h) return EFFECT_BAD_VALUE;
    if (!h->itfe || !h->itfe->command) return EFFECT_NO_INIT;

    return h->itfe->command(h, cmdCode, cmdSize, cmdData, replySize, replyData);
}

// Optional: chain-level convenience API to send command to all effects
effect_result_t chain_send_command(effect_chain_t* chain, uint32_t cmdCode, uint32_t cmdSize, void* cmdData) {
    if (!chain) return EFFECT_BAD_VALUE;
    effect_chain_wrapper_t* w = (effect_chain_wrapper_t*)((char*)chain - offsetof(effect_chain_wrapper_t, chain));
    if (!w) return EFFECT_BAD_VALUE;

    pthread_mutex_lock(&w->lock);
    effect_node_t* cur = w->chain.head;
    while (cur) {
        effect_handle_t* h = cur->effect;
        if (h && h->itfe && h->itfe->command) {
            uint32_t reply = 0;
            effect_result_t r = dispatch_command_to_effect(h, cmdCode, cmdSize, cmdData, &reply, NULL);
            if (r != EFFECT_OK) {
                pthread_mutex_unlock(&w->lock);
                return r;
            }
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&w->lock);
    return EFFECT_OK;
}

// End of file