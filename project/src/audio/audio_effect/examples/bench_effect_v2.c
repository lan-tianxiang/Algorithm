#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>

#include "../plugin_loader_v2.h"
#include "../hd_audio_effect_v2.h"

static double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char** argv) {
    int num_effects = 10;
    int iterations = 20000;
    if (argc > 1) num_effects = atoi(argv[1]);
    if (argc > 2) iterations = atoi(argv[2]);

    printf("bench: effects=%d iter=%d\n", num_effects, iterations);

    // load single plugin and create multiples via the plugin's create function
    effect_handle_v2_t* base = NULL;
    plugin_v2_t* ph = plugin_v2_load("./libhd_audio_gain_v2.so", &base);
    if (!ph) { fprintf(stderr, "load failed\n"); return 1; }

    effect_chain_manager_t* mgr = effect_chain_manager_create();
    effect_handle_v2_t** arr = malloc(sizeof(effect_handle_v2_t*) * num_effects);
    arr[0] = base;
    for (int i = 1; i < num_effects; ++i) {
        // try to create new instance via vtable if available
        effect_handle_v2_t* inst = NULL;
        if (base->vtable && base->vtable->create) base->vtable->create(&inst, &base->desc);
        if (!inst) inst = base; // fallback reuse
        arr[i] = inst;
    }

    for (int i = 0; i < num_effects; ++i) effect_chain_manager_add(mgr, arr[i]);

    // synthetic buffer
    const uint32_t frames = 480;
    const uint16_t ch = 2;
    int16_t* inbuf = malloc(sizeof(int16_t) * frames * ch);
    int16_t* outbuf = malloc(sizeof(int16_t) * frames * ch);
    memset(inbuf, 0x20, sizeof(int16_t) * frames * ch);
    audio_buffer_v2_t in = { .data = inbuf, .frames = frames, .rate = 48000, .channels = ch, .stride = ch * 2 };
    audio_buffer_v2_t out = { .data = outbuf };

    // bind to cpu 0 to reduce scheduler noise
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

    // prevent optimizer removing work
    volatile long long checksum = 0;

    double t0 = now_sec();
    for (int it = 0; it < iterations; ++it) {
        const effect_chain_snapshot_t* snap = effect_chain_get_snapshot(mgr);
        if (snap) {
            for (uint32_t i = 0; i < snap->num_effects; ++i) {
                effect_handle_v2_t* h = snap->effects[i];
                if (h && h->vtable && h->vtable->process && h->enabled) h->vtable->process(h, &in, &out);
            }
            // incorporate some output into checksum
            int16_t* outp = (int16_t*)out.data;
            for (uint32_t s = 0; s < frames * ch; ++s) checksum += outp[s];
            effect_chain_release_snapshot(snap);
        }
    }
    double t1 = now_sec();
    double secs = t1 - t0;
    printf("elapsed: %.6fs, throughput: %.0f callbacks/s\n", secs, iterations / secs);
    printf("checksum: %lld\n", checksum);

    // cleanup
    for (int i = 0; i < num_effects; ++i) effect_chain_manager_remove(mgr, arr[i]);
    plugin_v2_unload(ph, base);
    free(arr);
    free(inbuf);
    free(outbuf);
    effect_chain_manager_destroy(mgr);
    return 0;
}
