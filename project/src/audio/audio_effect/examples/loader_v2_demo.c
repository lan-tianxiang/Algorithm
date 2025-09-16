// loader_v2_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

#include "../src/audio/audio_effect/hd_audio_effect_v2.h"
#include "../src/audio/audio_effect/plugin_loader_v2.h"

typedef struct {
    effect_chain_manager_t* mgr;
    int running;
} audio_thread_arg_t;

void* audio_thread_func(void* arg) {
    audio_thread_arg_t* a = (audio_thread_arg_t*)arg;
    effect_chain_manager_t* mgr = a->mgr;
    // synthetic buffer
    const uint32_t frames = 128;
    const uint16_t ch = 2;
    int16_t inbuf[frames * ch];
    int16_t outbuf[frames * ch];
    memset(inbuf, 0x10, sizeof(inbuf));

    audio_buffer_v2_t in = { .data = inbuf, .frames = frames, .rate = 48000, .channels = ch, .stride = ch * 2 };
    audio_buffer_v2_t out = { .data = outbuf, .frames = 0, .rate = 0, .channels = 0, .stride = 0 };

    while (a->running) {
        const effect_chain_snapshot_t* snap = effect_chain_get_snapshot(mgr);
        if (snap && snap->num_effects > 0) {
            for (uint32_t i = 0; i < snap->num_effects; ++i) {
                effect_handle_v2_t* h = snap->effects[i];
                if (h && h->vtable && h->vtable->process && h->enabled) {
                    h->vtable->process(h, &in, &out);
                }
            }
        }
    sched_yield();
    }
    return NULL;
}

int main(int argc, char** argv) {
    const char* plugin_path = "./libhd_audio_gain_v2.so";
    effect_handle_v2_t* effect = NULL;
    plugin_v2_t* ph = plugin_v2_load(plugin_path, &effect);
    if (!ph) {
        fprintf(stderr, "failed to load plugin %s\n", plugin_path);
        return 1;
    }
    effect_chain_manager_t* mgr = effect_chain_manager_create();
    if (!mgr) return 2;
    effect_chain_manager_add(mgr, effect);

    audio_thread_arg_t arg = { .mgr = mgr, .running = 1 };
    pthread_t at;
    pthread_create(&at, NULL, audio_thread_func, &arg);

    printf("Running demo for 2 seconds...\n");
    sleep(2);
    arg.running = 0;
    pthread_join(at, NULL);

    effect_chain_manager_remove(mgr, effect);
    plugin_v2_unload(ph, effect);
    effect_chain_manager_destroy(mgr);
    return 0;
}
