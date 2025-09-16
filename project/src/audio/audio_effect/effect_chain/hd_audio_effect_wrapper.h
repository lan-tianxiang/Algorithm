/* hd_audio_effect_wrapper.h - wrapper to provide stable API and forward to legacy or v2 implementations */
#ifndef HD_AUDIO_EFFECT_WRAPPER_H
#define HD_AUDIO_EFFECT_WRAPPER_H

#if defined(AUDIO_EFFECT_USE_V2)
#include "hd_audio_effect_v2.h"
#elif defined(AUDIO_EFFECT_USE_V1)
#include "hd_audio_effect_v1.h"
#endif

effect_chain_t* audio_create_effect_chain(void);
effect_result_t audio_destroy_effect_chain(effect_chain_t* chain);
effect_result_t audio_add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect);
effect_result_t audio_remove_effect_from_chain(effect_chain_t* chain, effect_handle_t* effect);
effect_result_t audio_process_audio_chain(effect_chain_t* chain, audio_buffer_t* in, audio_buffer_t* out);
effect_result_t audio_set_effect_enabled(effect_chain_t* chain, effect_handle_t* effect, int enabled);
effect_result_t audio_get_chain_effects(effect_chain_t* chain, effect_handle_t** out_list, size_t max, size_t* out_count);

// helper: utility to compare/print uuid
int effect_uuid_compare(const effect_uuid_t* a, const effect_uuid_t* b);
void effect_uuid_copy(effect_uuid_t* dst, const effect_uuid_t* src);
#endif /* HD_AUDIO_EFFECT_WRAPPER_H */
