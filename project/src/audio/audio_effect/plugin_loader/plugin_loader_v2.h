/* plugin_loader_v2.h */
#ifndef PLUGIN_LOADER_V2_H
#define PLUGIN_LOADER_V2_H

#include "hd_audio_effect_v2.h"

typedef struct plugin_v2_s plugin_v2_t;

plugin_v2_t* plugin_v2_load(const char* path, effect_handle_v2_t** out);
void plugin_v2_unload(plugin_v2_t* ph, effect_handle_v2_t* h);

#endif // PLUGIN_LOADER_V2_H
