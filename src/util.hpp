#ifndef GODOTPD_UTIL_HPP
#define GODOTPD_UTIL_HPP

#include <godot_cpp/variant/string.hpp>
#include <string>

inline godot::String godot_string_from(std::string from) {
    return from.c_str();
}

inline std::string std_string_from(godot::String from) {
    return from.utf8().get_data();
}

#endif