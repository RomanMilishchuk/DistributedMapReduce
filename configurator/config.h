#ifndef MAP_REDUCE_CONFIG_H
#define MAP_REDUCE_CONFIG_H

#include <filesystem>
#include <dlfcn.h>

std::shared_ptr<void> get_config_dll_handler(const std::filesystem::path &path) {
    void *library_handler = dlopen("libmap_reduce_config.so", RTLD_LAZY);
    if (library_handler == nullptr) {
        throw std::runtime_error(dlerror());
    }
    return std::shared_ptr<void>(library_handler, [](void *handle) {
        dlclose(handle);
    });
}

JobConfig *get_config(std::shared_ptr<void> config_dll_handler) {
    return ((get_config_t) dlsym(config_dll_handler.get(), "get_config"))();
}

#endif //MAP_REDUCE_CONFIG_H
