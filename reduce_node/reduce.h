#ifndef MAP_REDUCE_REDUCE_H
#define MAP_REDUCE_REDUCE_H

#include <iostream>
#include <string>

#include "concurrent_queue.h"
#include "../configurator/JobConfig.h"
#include "../configurator/config.h"
#include "../util.h"

template<typename T>
struct ptr_less {
    bool operator()(const T &lhs, const T &rhs) const {
        return *lhs.get() < *(rhs).get();
    }
};

std::map<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>, ptr_less<std::unique_ptr<KeyValueType>>> key_values;
ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>> queue;

constexpr int map_cnt = 4;


void
reduce(ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>> &q) {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);
    auto[key, values] = q.pop();
    auto[key_out, value_out] = cfg->reduce_class->reduce(key, values);
    std::cout << key_out->to_string() << " " << value_out->to_string() << std::endl;
    //TODO: Add sending to master node result
}

void process(const std::string &json) {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);
    auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
    auto it = key_values.find(key);
    if (it == key_values.end()) {
        if (map_cnt == 1) {
            auto values = std::vector<std::unique_ptr<KeyValueType>>{};
            values.push_back(std::move(value));
            queue.push(make_pair(std::move(key), std::move(values)));
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    } else {
        if (it->second.size() == map_cnt - 1) {
            auto values = std::move(it->second);
            values.push_back(std::move(value));
            key_values.erase(it);
            queue.push(make_pair(std::move(key), std::move(values)));
            //TODO: make actually concurrent
            reduce(queue);
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    }
}

#endif //MAP_REDUCE_REDUCE_H
