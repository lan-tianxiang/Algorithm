#ifndef HD_AUDIO_GAIN_H
#define HD_AUDIO_GAIN_H

#include "hd_audio_effect.h"

// 增益效果结构体
typedef struct {
    effect_handle_t handle;
    float gain;
} gain_effect_t;

#endif // AUDIO_GAIN_H