(Audio effect module)

This directory implements an AOSP-like audio effect chain and a small plugin ABI.

ABI summary
 - Header: `hd_audio_effect.h` defines types and commands.
 - Plugins should export a create function with one of the names tried by the loader:
		 - `effect_create` (generic)
		 - `effect_create_<name>` (e.g. `effect_create_gain`)
	 Signature: `effect_result_t effect_create(..., const effect_descriptor_t* desc)`

 - Plugins should export a release function:
		 - `effect_release` or `effect_release_<name>`
	 Signature: `effect_result_t effect_release(effect_handle_t* h)`

Loader
 - A simple loader is provided in `plugin_loader.c`/`.h`. Use `plugin_load(path, desc, &out_effect)` to load and instantiate.
 - Use `plugin_unload(ph, effect)` to release and unload.

Building plugins
 - Plugins should be built as shared libraries (`-shared -fPIC`) and place exported symbols.

Example
 - See `effect_module/audio_gain/hd_audio_gain.c`: it exports `effect_create_gain` and `effect_release_gain`.

