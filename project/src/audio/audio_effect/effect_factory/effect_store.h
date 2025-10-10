/*
    FILE NAME: effect_store.h
    PURPOSE: Declarations for effect store arrays and counts
    DATE: 2025/9/16
    AUTHOR: nolan.lan (adapted)
*/

#ifndef EFFECT_STORE_H
#define EFFECT_STORE_H

#include "hd_audio_effect_wrapper.h"

typedef struct {
    const effect_interface_t* interface;
    audio_config_t input_config;
    audio_config_t output_config;
} effect_store_t;

typedef struct {
    effect_store_t* store;
    size_t count;
} effect_store_bussness_t;

typedef enum {
    EFFECT_STORE_BUSSNESS_TX = 0,
    EFFECT_STORE_BUSSNESS_RX = 1,
    EFFECT_STORE_BUSSNESS_CUSTOM,
    EFFECT_STORE_BUSSNESS_MAX,
} effect_store_bussness_id_t;

effect_store_bussness_t* get_effect_store_bussness(effect_store_bussness_id_t id);

#endif // EFFECT_STORE_H