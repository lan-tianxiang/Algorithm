#include "hd_pass_through.h"
#include "hd_audio_effect_wrapper.h"

// Provide a small default effect implementation for testing: passthrough
effect_result_t passthrough_process(effect_handle_t* self, audio_buffer_t* in, audio_buffer_t* out) {
    if (!in || !out) return EFFECT_BAD_VALUE;
    // simple deep copy of buffer pointer (no format conversion)
    uint32_t bytes_to_copy = in->frameCount * in->channelCount * in->stride;
    memcpy(out->buffer, in->buffer, bytes_to_copy);
    out->frameCount = in->frameCount;
    out->sampleRate = in->sampleRate;
    out->channelCount = in->channelCount;
    out->stride = in->stride;
    out->format = in->format;
    return EFFECT_OK;
}

effect_result_t passthrough_command(effect_handle_t* self, uint32_t cmdCode, uint32_t cmdSize, void* cmdData,
                                   uint32_t* replySize, void* replyData) {
    if (!self) return EFFECT_BAD_VALUE;
    switch (cmdCode) {
        case EFFECT_CMD_ENABLE:
            self->enabled = 1;
            return EFFECT_OK;
        case EFFECT_CMD_DISABLE:
            self->enabled = 0;
            return EFFECT_OK;
        case EFFECT_CMD_SET_PARAM:
            // simple store to instance_data if provided (caller must manage memory)
            return EFFECT_OK;
        default:
            return EFFECT_ERROR;
    }
}

effect_result_t passthrough_release(effect_handle_t* self) {
    if (!self) return EFFECT_BAD_VALUE;
    free(self);
    return EFFECT_OK;
}

effect_result_t passthrough_create(effect_handle_t** out, const effect_descriptor_t* descriptor) {
    if (!out) return EFFECT_BAD_VALUE;
    effect_handle_t* h = (effect_handle_t*)calloc(1, sizeof(*h));
    if (!h) return EFFECT_NO_MEMORY;
    static const effect_interface_t iface = {
        .process = passthrough_process,
        .command = passthrough_command,
        .create = NULL,
        .release = passthrough_release,
        .get_descriptor = NULL,
    };
    h->itfe = &iface;
    if (descriptor) h->descriptor = *descriptor;
    h->enabled = 1;
    *out = h;
    return EFFECT_OK;
}

