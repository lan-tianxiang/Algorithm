/* plugin_loader_v2.c */
#include "plugin_loader_v2.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct plugin_v2_s { void* dl; char* path; };

plugin_v2_t* plugin_v2_load(const char* path, effect_handle_v2_t** out) {
    if (!path || !out) return NULL;
    void* dl = dlopen(path, RTLD_NOW);
    if (!dl) { fprintf(stderr, "dlopen failed: %s\n", dlerror()); return NULL; }

    typedef effect_result_t (*create_fn_t)(effect_handle_v2_t**, const effect_descriptor_v2_t*);
    create_fn_t create_fn = (create_fn_t)dlsym(dl, "effect_create_v2");
    if (!create_fn) { fprintf(stderr, "effect_create_v2 not found\n"); dlclose(dl); return NULL; }

    // create descriptor with ABI v2
    effect_descriptor_v2_t desc = {0};
    desc.api_version = HD_EFFECT_ABI_V2;
    effect_handle_v2_t* h = NULL;
    effect_result_t r = create_fn(&h, &desc);
    if (r != EFFECT_OK || !h) { fprintf(stderr, "plugin create failed\n"); dlclose(dl); return NULL; }

    // validate descriptor version
    effect_descriptor_v2_t got = {0};
    if (h->vtable && h->vtable->get_descriptor) h->vtable->get_descriptor(h, &got);
    if (got.api_version != HD_EFFECT_ABI_V2) {
        fprintf(stderr, "ABI version mismatch\n");
        if (h->vtable && h->vtable->release) h->vtable->release(h);
        dlclose(dl);
        return NULL;
    }

    plugin_v2_t* ph = malloc(sizeof(*ph));
    ph->dl = dl;
    ph->path = malloc(strlen(path) + 1);
    if (ph->path) strcpy(ph->path, path);
    *out = h;
    return ph;
}

void plugin_v2_unload(plugin_v2_t* ph, effect_handle_v2_t* h) {
    if (!ph) return;
    if (h && h->vtable && h->vtable->release) h->vtable->release(h);
    dlclose(ph->dl);
    free(ph->path);
    free(ph);
}
