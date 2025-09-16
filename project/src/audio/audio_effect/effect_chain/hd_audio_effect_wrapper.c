/* hd_audio_effect_wrapper.c - forward wrapper implementation */
#include "hd_audio_effect_wrapper.h"

effect_chain_t* audio_create_effect_chain(void) {
    return create_effect_chain();
}

effect_result_t audio_destroy_effect_chain(effect_chain_t* chain) {
    return destroy_effect_chain(chain);
}

effect_result_t audio_add_effect_to_chain(effect_chain_t* chain, effect_handle_t* effect) {
    return add_effect_to_chain(chain, effect);
}

effect_result_t audio_remove_effect_from_chain(effect_chain_t* chain, effect_handle_t* effect) {
    return remove_effect_from_chain(chain, effect);
}

effect_result_t audio_process_audio_chain(effect_chain_t* chain, audio_buffer_t* in, audio_buffer_t* out) {
    return process_audio_chain(chain, in, out);
}

effect_result_t audio_set_effect_enabled(effect_chain_t* chain, effect_handle_t* effect, int enabled) {
    return set_effect_enabled(chain, effect, enabled);
}

effect_result_t audio_get_chain_effects(effect_chain_t* chain, effect_handle_t** out_list, size_t max, size_t* out_count) {
    return get_chain_effects(chain, out_list, max, out_count);
}

// UUID helpers
int effect_uuid_compare(const effect_uuid_t* a, const effect_uuid_t* b) {
    return memcmp(a->data, b->data, sizeof(a->data));
}

void effect_uuid_copy(effect_uuid_t* dst, const effect_uuid_t* src) {
    memcpy(dst->data, src->data, sizeof(dst->data));
}
