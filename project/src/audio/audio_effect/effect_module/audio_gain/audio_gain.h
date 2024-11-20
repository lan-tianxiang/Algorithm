#ifndef AUDIO_GAIN_H
#define AUDIO_GAIN_H

#include "audio_effect.h"

// 增益效果结构体
typedef struct {
    effect_handle_t handle;
    float gain;
} gain_effect_t;

#endif // AUDIO_GAIN_H