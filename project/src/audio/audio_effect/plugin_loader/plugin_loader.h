/*
 * Simple plugin loader for audio effects
 */
#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include "hd_audio_effect.h"

typedef struct plugin_handle_s plugin_handle_t;

// load a plugin shared object at path, call its create symbol to instantiate effect
plugin_handle_t* plugin_load(const char* path, const effect_descriptor_t* desc, effect_handle_t** out_effect);
// release plugin and its effect
void plugin_unload(plugin_handle_t* ph, effect_handle_t* effect);

#endif // PLUGIN_LOADER_H
