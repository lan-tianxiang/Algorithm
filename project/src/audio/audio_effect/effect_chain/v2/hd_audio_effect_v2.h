/*
 * hd_audio_effect_v2.h
 * High-performance, AOSP-like audio effect chain v2
 * - ABI v2: lock-free read path for audio thread
 * - copy-on-write updates for chain modifications
 * - per-session/track insert points
 * - preallocated ring command queue for real-time commands
 * - strict real-time safety rules documented below
 */

#ifndef HD_AUDIO_EFFECT_V2_H
#define HD_AUDIO_EFFECT_V2_H

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

// ABI version
#define HD_EFFECT_ABI_V2 2

typedef int32_t effect_result_t;
#define EFFECT_OK 0
#define EFFECT_ERROR -1

// 128-bit UUID
typedef struct { uint8_t b[16]; } effect_uuid_t;

// Insert points (similar to Android pre/post/aux)
typedef enum {
    INSERT_POINT_PRE = 0,
    INSERT_POINT_POST = 1,
    INSERT_POINT_AUX = 2,
    INSERT_POINT_MAX
} insert_point_t;

// descriptor
typedef struct {
    effect_uuid_t type;
    effect_uuid_t uuid;
    uint32_t api_version; // equals HD_EFFECT_ABI_V2
    uint32_t flags;
    char name[64];
    char implementor[64];
} effect_descriptor_v2_t;

// audio buffer (real-time friendly)
typedef struct {
    void* data;           // pointer to raw frames (owner managed)
    uint32_t frames;      // frame count
    uint32_t rate;        // sample rate
    uint16_t channels;    // channel count
    uint16_t stride;      // bytes per frame
} audio_buffer_v2_t;

// command ring: preallocated, single-producer(single-controller)/single-consumer(audio thread)
typedef struct {
    uint32_t capacity;    // number of entries
    uint32_t head;        // consumer index
    uint32_t tail;        // producer index (updated by controller)
    uint32_t mask;        // capacity-1 (power of two)
    void** entries;       // pointers to commands (opaque)
} cmd_ring_t;

// effect vtable
typedef struct effect_handle_v2_s effect_handle_v2_t;

typedef struct {
    // process MUST be real-time safe: no locks, no malloc, no syscalls
    effect_result_t (*process)(effect_handle_v2_t* h, const audio_buffer_v2_t* in, audio_buffer_v2_t* out);
    // command may be called from controller thread; should queue into per-effect preallocated work area
    effect_result_t (*command)(effect_handle_v2_t* h, uint32_t cmd, uint32_t size, const void* data);
    effect_result_t (*create)(effect_handle_v2_t** out, const effect_descriptor_v2_t* desc);
    effect_result_t (*release)(effect_handle_v2_t* h);
    effect_result_t (*get_descriptor)(effect_handle_v2_t* h, effect_descriptor_v2_t* out);
} effect_interface_v2_t;

struct effect_handle_v2_s {
    const effect_interface_v2_t* vtable;
    effect_descriptor_v2_t desc;
    int enabled;
    insert_point_t insert_point;
    int priority; // higher -> earlier
    // runtime work area: pointer to preallocated memory for real-time operations
    void* rt_work;
    // private data for implementation
    void* impl;
};

// Effect chain snapshot (immutable once published). Audio thread reads a pointer to this snapshot.
typedef struct effect_chain_snapshot_s {
    atomic_uint refcount; // reference count for RCU-style reclamation
    uint32_t num_effects;
    effect_handle_v2_t** effects; // pointer to array of pointers (immutable)
} effect_chain_snapshot_t;

// chain manager (controller side) maintains current snapshot pointer and uses copy-on-write
typedef struct effect_chain_manager_s effect_chain_manager_t;

// APIs
// create manager
effect_chain_manager_t* effect_chain_manager_create(void);
void effect_chain_manager_destroy(effect_chain_manager_t* m);

// controller operations (copy-on-write)
effect_result_t effect_chain_manager_add(effect_chain_manager_t* m, effect_handle_v2_t* h);
effect_result_t effect_chain_manager_remove(effect_chain_manager_t* m, effect_handle_v2_t* h);
effect_result_t effect_chain_manager_set_enabled(effect_chain_manager_t* m, effect_handle_v2_t* h, int enabled);

// publish snapshot atomically for audio thread to read
effect_chain_snapshot_t* effect_chain_manager_publish(effect_chain_manager_t* m);

// audio thread APIs
// obtain pointer to current snapshot (no lock, must be used quickly) - returns NULL if none
const effect_chain_snapshot_t* effect_chain_get_snapshot(effect_chain_manager_t* m);

// release snapshot reference (called by audio thread when done)
void effect_chain_release_snapshot(const effect_chain_snapshot_t* snap);

// command ring helpers
cmd_ring_t* cmd_ring_create(uint32_t capacity_power_of_two);
void cmd_ring_destroy(cmd_ring_t* r);
int cmd_ring_push(cmd_ring_t* r, void* entry); // non-blocking: returns 0 on success, -1 if full
void* cmd_ring_pop(cmd_ring_t* r); // returns NULL if empty

#ifdef __cplusplus
}
#endif

#endif // HD_AUDIO_EFFECT_V2_H