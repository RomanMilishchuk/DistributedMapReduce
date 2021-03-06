// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_UTIL_H
#define MAP_REDUCE_UTIL_H

#include <vector>

#include <boost/asio.hpp>
#include "types/KeyValueType.h"
#include "types/KeyValueTypeFactory.h"


namespace map_reduce {
    std::pair<std::string, unsigned int> parse_ip_port(const std::string &address);

    std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
    get_key_values_from_csv(const std::string &data, std::unique_ptr<KeyValueTypeFactory> &key_factory,
                            std::unique_ptr<KeyValueTypeFactory> &value_factory, char delimiter = ',',
                            char end_of_line = '\n');

    std::string
    to_csv(const std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> &key_values,
           char delimeter = ',', char end_of_line = '\n');

    std::string to_json(const std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>> &key_value);

    std::string data_end_message();

    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    get_key_value_from_json(const std::string &data, std::unique_ptr<KeyValueTypeFactory> &key_factory,
                            std::unique_ptr<KeyValueTypeFactory> &value_factory);

    const std::string data_end_flag = "data_has_ended";

    class data_ended_error : public std::runtime_error {
    public:
        data_ended_error(const std::string &msg = "end of the data") : runtime_error(msg) {}
    };

    void send_end_message(const boost::asio::ip::tcp::endpoint &reduce_ep);

}
#endif //MAP_REDUCE_UTIL_H

