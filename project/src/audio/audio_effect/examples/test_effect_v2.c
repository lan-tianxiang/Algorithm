// test_effect_v2.c - functional tests for v2 effect chain
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../plugin_loader_v2.h"
#include "../hd_audio_effect_v2.h"

int main() {
    printf("v2 functional test start\n");
    effect_handle_v2_t* h = NULL;
    plugin_v2_t* ph = plugin_v2_load("./libhd_audio_gain_v2.so", &h);
    if (!ph) { printf("failed to load plugin\n"); return 1; }
    effect_chain_manager_t* mgr = effect_chain_manager_create();
    if (!mgr) { printf("failed to create mgr\n"); return 2; }

    // add
    if (effect_chain_manager_add(mgr, h) != EFFECT_OK) { printf("add failed\n"); return 3; }
    printf("added effect\n");

    // enable/disable
    effect_chain_manager_set_enabled(mgr, h, 0);
    printf("disabled effect\n");
    effect_chain_manager_set_enabled(mgr, h, 1);
    printf("enabled effect\n");

    // remove
    effect_chain_manager_remove(mgr, h);
    printf("removed effect\n");

    plugin_v2_unload(ph, h);
    effect_chain_manager_destroy(mgr);
    printf("v2 functional test passed\n");
    return 0;
}
