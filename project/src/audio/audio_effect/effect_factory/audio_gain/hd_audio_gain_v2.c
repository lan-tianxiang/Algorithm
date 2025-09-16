/*
 * hd_audio_gain_v2.c
 * v2 example gain effect implementing hd_audio_effect_v2.h ABI
 */

#include "hd_audio_gain.h"
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>

typedef struct {
    _Atomic float gain; // real-time safe atomic gain value
} gain_rt_work_t;

typedef struct {
    effect_handle_v2_t base;
    gain_rt_work_t* rt;
    // controller-side mailbox: new_gain pointer (allocated on controller stack or heap)
    _Atomic(float*) mailbox;
} gain_effect_v2_t;

static effect_result_t gain_process(effect_handle_v2_t* h, const audio_buffer_v2_t* in, audio_buffer_v2_t* out) {
    if (!h || !in || !out) return EFFECT_ERROR;
    gain_effect_v2_t* g = (gain_effect_v2_t*)h;
    // apply any pending mailbox (controller -> audio thread) by atomic swap
    float* pending = atomic_exchange(&g->mailbox, NULL);
    if (pending) {
        atomic_store(&g->rt->gain, *pending);
        free(pending); // controller allocated; safe to free
    }

    // simple passthrough with gain
    uint32_t frames = in->frames;
    uint16_t ch = in->channels;
    float gain = atomic_load(&g->rt->gain);
    // assume 16-bit PCM
    int16_t* inbuf = (int16_t*)in->data;
    int16_t* outbuf = (int16_t*)out->data;
    uint32_t samples = frames * ch;
    for (uint32_t i = 0; i < samples; ++i) {
        int32_t v = (int32_t)(inbuf[i] * gain);
        if (v > INT16_MAX) v = INT16_MAX;
        if (v < INT16_MIN) v = INT16_MIN;
        outbuf[i] = (int16_t)v;
    }
    out->frames = in->frames;
    out->rate = in->rate;
    out->channels = in->channels;
    out->stride = in->stride;
    return EFFECT_OK;
}

static effect_result_t gain_command(effect_handle_v2_t* h, uint32_t cmd, uint32_t size, const void* data) {
    if (!h) return EFFECT_ERROR;
    gain_effect_v2_t* g = (gain_effect_v2_t*)h;
    if (cmd == 1) { // set gain (cmd id 1)
        if (size < sizeof(float) || !data) return EFFECT_ERROR;
        float* p = malloc(sizeof(float));
        if (!p) return EFFECT_ERROR;
        memcpy(p, data, sizeof(float));
        float* prev = atomic_exchange(&g->mailbox, p);
        if (prev) free(prev);
        return EFFECT_OK;
    }
    return EFFECT_ERROR;
}

static effect_result_t gain_release(effect_handle_v2_t* h);
static effect_result_t gain_get_descriptor(effect_handle_v2_t* h, effect_descriptor_v2_t* out);

static effect_result_t gain_create(effect_handle_v2_t** out, const effect_descriptor_v2_t* desc) {
    if (!out) return EFFECT_ERROR;
    gain_effect_v2_t* g = calloc(1, sizeof(*g));
    if (!g) return EFFECT_ERROR;
    static const effect_interface_v2_t iface = {
        .process = gain_process,
        .command = gain_command,
        .create = gain_create,
        .release = NULL, // set below
        .get_descriptor = NULL, // set below
    };
    g->base.vtable = &iface;
    g->rt = malloc(sizeof(gain_rt_work_t));
    if (!g->rt) { free(g); return EFFECT_ERROR; }
    atomic_store(&g->rt->gain, 1.0f);
    atomic_store(&g->mailbox, NULL);
    // set descriptor
    if (desc) g->base.desc = *desc;
    g->base.desc.api_version = HD_EFFECT_ABI_V2;
    // set vtable release/get_descriptor via mutable pointer hack
    *((effect_interface_v2_t**)&g->base.vtable) = (effect_interface_v2_t*)&iface;
    // we must set release and get_descriptor (can't modify static const), so create a mutable copy
    effect_interface_v2_t* mv = malloc(sizeof(*mv));
    if (!mv) { free(g->rt); free(g); return EFFECT_ERROR; }
    *mv = iface;
    mv->release = gain_release;
    mv->get_descriptor = gain_get_descriptor;
    g->base.vtable = mv;
    *out = &g->base;
    return EFFECT_OK;
}

static effect_result_t gain_release(effect_handle_v2_t* h) {
    if (!h) return EFFECT_ERROR;
    gain_effect_v2_t* g = (gain_effect_v2_t*)h;
    float* pending = atomic_exchange(&g->mailbox, NULL);
    if (pending) free(pending);
    if (g->rt) free(g->rt);
    // free mutable vtable if allocated
    if (g->base.vtable) free((void*)g->base.vtable);
    free(g);
    return EFFECT_OK;
}

static effect_result_t gain_get_descriptor(effect_handle_v2_t* h, effect_descriptor_v2_t* out) {
    if (!h || !out) return EFFECT_ERROR;
    *out = h->desc;
    return EFFECT_OK;
}

// exported symbols for loader v2
__attribute__((visibility("default")))
effect_result_t effect_create_v2(effect_handle_v2_t** out, const effect_descriptor_v2_t* desc) {
    return gain_create(out, desc);
}

__attribute__((visibility("default")))
effect_result_t effect_release_v2(effect_handle_v2_t* h) {
    return gain_release(h);
}
