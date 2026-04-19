#ifndef TEST_STREAM_OPERATORS_HPP
#define TEST_STREAM_OPERATORS_HPP

#include <ostream>
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @brief Output stream operator for vector types
 * Used to print vector contents in test failure messages
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}

/**
 * @brief Output stream operator for unordered_map types
 * Used to print map contents in test failure messages
 */
template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& map) {
    os << "{";
    bool first = true;
    for (const auto& pair : map) {
        if (!first) os << ", ";
        os << pair.first << ": " << pair.second;
        first = false;
    }
    os << "}";
    return os;
}

#endif // TEST_STREAM_OPERATORS_HPP
