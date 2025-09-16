/*
 * plugin_loader.c
 * A tiny loader that dlopen's a plugin and invokes its create function.
 */

#include "plugin_loader.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct plugin_handle_s {
    void* dl;
    char* path;
};

// symbol names we try in order
static const char* create_symbols[] = {"effect_create", "effect_create_gain", NULL};
static const char* release_symbols[] = {"effect_release", "effect_release_gain", NULL};

plugin_handle_t* plugin_load(const char* path, const effect_descriptor_t* desc, effect_handle_t** out_effect) {
    if (!path || !out_effect) return NULL;
    void* dl = dlopen(path, RTLD_NOW);
    if (!dl) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return NULL;
    }

    typedef effect_result_t (*create_fn_t)(effect_handle_t**, const effect_descriptor_t*);
    create_fn_t create_fn = NULL;
    for (const char** s = create_symbols; *s; ++s) {
        create_fn = (create_fn_t)dlsym(dl, *s);
        if (create_fn) break;
    }
    if (!create_fn) {
        fprintf(stderr, "create symbol not found in %s\n", path);
        dlclose(dl);
        return NULL;
    }

    effect_handle_t* effect = NULL;
    effect_result_t r = create_fn(&effect, desc);
    if (r != EFFECT_OK || !effect) {
        fprintf(stderr, "plugin create failed\n");
        dlclose(dl);
        return NULL;
    }

    plugin_handle_t* ph = (plugin_handle_t*)malloc(sizeof(*ph));
    ph->dl = dl;
    ph->path = (char*)malloc(strlen(path) + 1);
    if (ph->path) strcpy(ph->path, path);
    *out_effect = effect;
    return ph;
}

void plugin_unload(plugin_handle_t* ph, effect_handle_t* effect) {
    if (!ph) return;
    if (effect) {
        // try to find release symbol
        typedef effect_result_t (*release_fn_t)(effect_handle_t*);
        release_fn_t release_fn = NULL;
        for (const char** s = release_symbols; *s; ++s) {
        release_fn = (release_fn_t)dlsym(ph->dl, *s);
            if (release_fn) break;
        }
        if (release_fn) release_fn(effect);
    }
    dlclose(ph->dl);
    free(ph->path);
    free(ph);
}
