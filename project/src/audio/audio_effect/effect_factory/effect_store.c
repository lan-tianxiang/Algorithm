/*
    FILE NAME: effect_store.c
    PURPOSE: Implementation of effect store
    DATE: 2025/9/16
    AUTHOR: nolan.lan (adapted)
*/

#include "effect_store.h"

extern const effect_interface_t g_gain_interface;
extern const effect_interface_t g_pass_through_interface;

static effect_store_t g_tx_effect_store[] = {
    {
        &g_gain_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
    {
        &g_pass_through_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
    {
        &g_gain_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
};

static effect_store_t g_rx_effect_store[] = {
    {
        &g_gain_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
    {
        &g_pass_through_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
    {
        &g_gain_interface,
        .input_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
        .output_config = {
            .format = PCM_FORMAT_FLOAT_LE,
            .sampleRate = 44100,
            .channelCount = 2,
            .stride = sizeof(float),
        },
    },
};

static effect_store_bussness_t g_effect_store_bussness[EFFECT_STORE_BUSSNESS_MAX] = {
    {
        .store = g_tx_effect_store,
        .count = sizeof(g_tx_effect_store) / sizeof(g_tx_effect_store[0]),
    },
    {
        .store = g_rx_effect_store,
        .count = sizeof(g_rx_effect_store) / sizeof(g_rx_effect_store[0]),
    },
};

effect_store_bussness_t* get_effect_store_bussness(effect_store_bussness_id_t id) {
    if (id < 0 || id >= EFFECT_STORE_BUSSNESS_MAX) {
        return NULL;
    }
    return &g_effect_store_bussness[id];
}