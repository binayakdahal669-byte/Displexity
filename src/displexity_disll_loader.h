#ifndef DISPLEXITY_DISLL_LOADER_H
#define DISPLEXITY_DISLL_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

// Initialize runtime discovery of installed .disll libraries.
// The runtime can call this at startup with the directory to search
// (typically "bin/disll"). Implementations should be provided in
// the runtime source to actually load platform-specific dynamic libs.
void displexity_load_dislls_from_bindir(const char *bindir);

#ifdef __cplusplus
}
#endif

#endif // DISPLEXITY_DISLL_LOADER_H
