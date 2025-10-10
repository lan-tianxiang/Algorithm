/*
    FILE NAME: hd_audio_effect_v2.c
    PURPOSE: Implementation of high-performance audio effect chain v2
    DATE: 2025/9/16
    AUTHOR: nolan.lan (adapted)
*/

#include "hd_audio_effect_v2.h"
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

// chain manager definition
struct effect_chain_manager_s {
    _Atomic(effect_chain_snapshot_t*) current; // audio thread reads this pointer
};

effect_chain_manager_t* effect_chain_manager_create(void) {
    effect_chain_manager_t* m = calloc(1, sizeof(*m));
    if (!m) return NULL;
    atomic_store(&m->current, NULL);
    return m;
}

void effect_chain_manager_destroy(effect_chain_manager_t* m) {
    if (!m) return;
    effect_chain_snapshot_t* snap = atomic_load(&m->current);
    if (snap) {
        // decrement refcount and free if zero
        if (atomic_fetch_sub(&snap->refcount, 1) == 1) {
            free(snap->effects);
            free(snap);
        }
    }
    free(m);
}

static effect_chain_snapshot_t* snapshot_clone_and_add_rcu(effect_chain_snapshot_t* old, effect_handle_v2_t* h) {
    uint32_t old_n = old ? old->num_effects : 0;
    effect_chain_snapshot_t* ns = malloc(sizeof(*ns));
    if (!ns) return NULL;
    atomic_init(&ns->refcount, 1);
    ns->num_effects = old_n + 1;
    ns->effects = malloc(sizeof(effect_handle_v2_t*) * ns->num_effects);
    if (!ns->effects) { free(ns); return NULL; }
    for (uint32_t i = 0; i < old_n; ++i) ns->effects[i] = old->effects[i];
    ns->effects[old_n] = h;
    return ns;
}

static effect_chain_snapshot_t* snapshot_clone_and_remove_rcu(effect_chain_snapshot_t* old, effect_handle_v2_t* h) {
    if (!old) return NULL;
    uint32_t old_n = old->num_effects;
    uint32_t keep = 0;
    for (uint32_t i = 0; i < old_n; ++i) if (old->effects[i] != h) keep++;
    effect_chain_snapshot_t* ns = malloc(sizeof(*ns));
    if (!ns) return NULL;
    atomic_init(&ns->refcount, 1);
    ns->num_effects = keep;
    ns->effects = malloc(sizeof(effect_handle_v2_t*) * keep);
    if (!ns->effects) { free(ns); return NULL; }
    uint32_t idx = 0;
    for (uint32_t i = 0; i < old_n; ++i) if (old->effects[i] != h) ns->effects[idx++] = old->effects[i];
    return ns;
}

effect_result_t effect_chain_manager_add(effect_chain_manager_t* m, effect_handle_v2_t* h) {
    if (!m || !h) return EFFECT_ERROR;
    effect_chain_snapshot_t* old = atomic_load(&m->current);
    effect_chain_snapshot_t* ns = snapshot_clone_and_add_rcu(old, h);
    if (!ns) return EFFECT_ERROR;
    effect_chain_snapshot_t* prev = atomic_exchange(&m->current, ns);
    if (prev) {
        if (atomic_fetch_sub(&prev->refcount, 1) == 1) {
            free(prev->effects);
            free(prev);
        }
    }
    return EFFECT_OK;
}

effect_result_t effect_chain_manager_remove(effect_chain_manager_t* m, effect_handle_v2_t* h) {
    if (!m || !h) return EFFECT_ERROR;
    effect_chain_snapshot_t* old = atomic_load(&m->current);
    effect_chain_snapshot_t* ns = snapshot_clone_and_remove_rcu(old, h);
    if (!ns) return EFFECT_ERROR;
    effect_chain_snapshot_t* prev = atomic_exchange(&m->current, ns);
    if (prev) {
        if (atomic_fetch_sub(&prev->refcount, 1) == 1) {
            free(prev->effects);
            free(prev);
        }
    }
    return EFFECT_OK;
}

effect_result_t effect_chain_manager_set_enabled(effect_chain_manager_t* m, effect_handle_v2_t* h, int enabled) {
    if (!m || !h) return EFFECT_ERROR;
    h->enabled = enabled ? 1 : 0;
    return EFFECT_OK;
}

effect_chain_snapshot_t* effect_chain_manager_publish(effect_chain_manager_t* m) {
    if (!m) return NULL;
    effect_chain_snapshot_t* snap = atomic_load(&m->current);
    if (snap) atomic_fetch_add(&snap->refcount, 1);
    return snap;
}

const effect_chain_snapshot_t* effect_chain_get_snapshot(effect_chain_manager_t* m) {
    if (!m) return NULL;
    effect_chain_snapshot_t* snap = atomic_load(&m->current);
    if (snap) atomic_fetch_add(&snap->refcount, 1);
    return snap;
}

void effect_chain_release_snapshot(const effect_chain_snapshot_t* snap) {
    if (!snap) return;
    if (atomic_fetch_sub(&((effect_chain_snapshot_t*)snap)->refcount, 1) == 1) {
        free(((effect_chain_snapshot_t*)snap)->effects);
        free((void*)snap);
    }
}

// cmd ring implementation
cmd_ring_t* cmd_ring_create(uint32_t capacity_power_of_two) {
    if (capacity_power_of_two == 0) return NULL;
    cmd_ring_t* r = malloc(sizeof(*r));
    if (!r) return NULL;
    r->capacity = 1u << capacity_power_of_two;
    r->mask = r->capacity - 1;
    r->head = 0;
    r->tail = 0;
    r->entries = calloc(r->capacity, sizeof(void*));
    if (!r->entries) { free(r); return NULL; }
    return r;
}

void cmd_ring_destroy(cmd_ring_t* r) {
    if (!r) return;
    free(r->entries);
    free(r);
}

int cmd_ring_push(cmd_ring_t* r, void* entry) {
    uint32_t tail = atomic_load_explicit((_Atomic uint32_t*)&r->tail, memory_order_relaxed);
    uint32_t head = atomic_load_explicit((_Atomic uint32_t*)&r->head, memory_order_acquire);
    if (((tail + 1) & r->mask) == (head & r->mask)) return -1; // full
    r->entries[tail & r->mask] = entry;
    atomic_store_explicit((_Atomic uint32_t*)&r->tail, tail + 1, memory_order_release);
    return 0;
}

void* cmd_ring_pop(cmd_ring_t* r) {
    uint32_t head = atomic_load_explicit((_Atomic uint32_t*)&r->head, memory_order_relaxed);
    uint32_t tail = atomic_load_explicit((_Atomic uint32_t*)&r->tail, memory_order_acquire);
    if ((head & r->mask) == (tail & r->mask)) return NULL; // empty
    void* e = r->entries[head & r->mask];
    atomic_store_explicit((_Atomic uint32_t*)&r->head, head + 1, memory_order_release);
    return e;
}
